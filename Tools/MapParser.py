import argparse
import random
import traceback

# Stores information about a group of blocks, i.e. a candidate for exporting as a single object.
# X positions are provided as a block offset from the leftmost column of blocks.
# Y positions are provided as a block offset from the topmost row of blocks, with more positive values being lower down.
class GroupInfo:
    def __init__(self, symbol=None, leftX=0, topY=0, dimX=0, dimY=0):
        self.symbol = symbol    # Number, String, w/e, used to identify the type of block in this group.
        self.leftX = leftX
        self.topY = topY
        self.dimX = dimX
        self.dimY = dimY

    def __repr__(self):
        return "%s(%r)" % (self.__class__, self.__dict__)
  
# Parse an input map into a format that can be read by the game engine.
class MapParser:
    def __init__(self):
        self.reset()

    def reset(self):
        self.activeGroups = set()       # Groups that aren't finalized yet - could still grow.
        self.exportGroups = set()       # Finalized groups - need to be written to output.

    def arrayToSet(self, a):
        self.reset()
        for idx, row in enumerate(a):
            rowGroups = self.parseLine(row, topY=idx)
            self.updateGroups(rowGroups)
        #print('\arrayToSet\nexportGroups: {}\nactiveGroups: {}'.format(self.exportGroups, self.activeGroups))
        # Combine any remaining active groups into the export list.
        self.exportGroups.update(self.activeGroups)
        self.activeGroups = set()

        return self.exportGroups

    # Take a set of groups and return an equivalent input array.
    # Return None if an error is encountered.
    def setToArray(self, groups):
        width = 0
        height = 0

        try:
            for group in groups:
                width = max(width, group.leftX + group.dimX)
                height = max(height, group.topY + group.dimY)

            array = []
            for h in range(height):
                array.append([None] * width)

            for group in groups:
                for x in range(group.leftX, group.leftX + group.dimX):
                    for y in range(group.topY, group.topY + group.dimY):
                        if array[y][x] is not None:
                            print('ERROR: Overlap in setToArray at ({},{})'.format(x, y))
                            return None
                        array[y][x] = group.symbol
        except:
            print('EXCEPTION in setToArray: {}'.format(traceback.format_exc()))
            return None

        return array

    # Parse a single line of the input description, provided as a list.
    def parseLine(self, lineSymbolList, topY=0):
        rowGroups = set()
        activeSymbol = None
        runLen = 0

        for idx, symbol in enumerate(lineSymbolList):
            # Form one-dimensional groups of items within this row.
            if symbol != activeSymbol:
                if runLen > 0:
                    rowGroups.add(GroupInfo(symbol=activeSymbol, leftX=idx-runLen, dimX=runLen, topY=topY, dimY=1))
                activeSymbol = symbol
                runLen = 0
            runLen += 1
        rowGroups.add(GroupInfo(symbol=symbol, leftX=idx+1-runLen, dimX=runLen, topY=topY, dimY=1))
        #print(rowGroups)
        return rowGroups

    # Update active and export groups based on new row group info.
    # Each group can be used in at most one overlap handling routine for simplicity.
    def updateGroups(self, rowGroups):
        #print('\nupdateGroups\nrowGroups: {}\nactiveGroups: {}'.format(rowGroups, self.activeGroups))
        newGroups = set()
        while rowGroups:
            rowGroup = rowGroups.pop()
            groupSymbol = rowGroup.symbol
            # Copy so we don't automatically export active group just for not matching the first row group.
            activeCopy = set(self.activeGroups)
            bHit = False
            while activeCopy:
                activeGroup = activeCopy.pop()
                if groupSymbol != activeGroup.symbol:
                    continue

                # First check if there's any overlap.
                rowLeft = rowGroup.leftX
                rowRight = rowLeft + rowGroup.dimX - 1
                activeLeft = activeGroup.leftX
                activeRight = activeLeft + activeGroup.dimX - 1
                bHit = (rowLeft <= activeRight) and (rowRight >= activeLeft)
                if bHit:
                    #print('Hit between row {} and active group {}'.format(rowGroup, activeGroup))

                    # Remove the group, in its old form, from active groups.
                    self.activeGroups.remove(activeGroup)

                    if rowLeft > activeLeft:
                        # Split off the left overhang portion of the active group.
                        #print('left overhang')
                        self.exportGroups.add(GroupInfo(symbol=groupSymbol, leftX=activeLeft, dimX=rowLeft-activeLeft, topY=activeGroup.topY, dimY=activeGroup.dimY))
                    if rowRight < activeRight:
                        # Split off the right overhang portion of the active group.
                        #print('right overhang')
                        self.exportGroups.add(GroupInfo(symbol=groupSymbol, leftX=rowRight+1, dimX=activeRight-rowRight, topY=activeGroup.topY, dimY=activeGroup.dimY))
                    if rowLeft < activeLeft:
                        # Row group extends past the active group's left edge. Split this off into a new group.
                        #print('left underhang')
                        newGroups.add(GroupInfo(symbol=groupSymbol, leftX=rowLeft, dimX=activeLeft-rowLeft,  topY=rowGroup.topY, dimY=1))
                    if rowRight > activeRight:
                        # Row group extends past the active group's right edge. Split this off into a new group.
                        #print('right underhang')
                        newGroups.add(GroupInfo(symbol=groupSymbol, leftX=activeRight+1, dimX=rowRight-activeRight, topY=rowGroup.topY, dimY=1))

                    # Handle the portion of the active group that will be extended by one row.
                    comboLeftX = max(rowLeft, activeLeft)
                    newGroups.add(GroupInfo(symbol=groupSymbol, leftX=comboLeftX, topY=activeGroup.topY, dimX=min(rowRight, activeRight)-comboLeftX+1, dimY=activeGroup.dimY+1))

                    break

            if not bHit:
                # If row group didn't combine with an active group, set it up to become its own active group.
                newGroups.add(rowGroup)

        # Any lingering active group didn't have a collision - these should be exported.
        self.exportGroups.update(self.activeGroups)

        # Update active groups with the latest batch
        self.activeGroups = newGroups
        #print('activeGroups: {}\nexportGroups: {}'.format(self.activeGroups, self.exportGroups))

def runTests(nRand, seed):
    ''' Run all unit tests, plus nRand random I/O tests. '''
    # (Name, Input 2D Array)
    # Since sets don't have an order, the output sets might change run-to-run, so can't test the set directly.
    # Instead, verify that the operation is reversible.
    STANDARD_TESTS = [
        ('Empty', []),
        ('OneEntry', [[0]]),
        ('OneRowSame', [[0, 0]]),
        ('OneRowDiff', [[1, 2]]),
        ('OneColumnSame', [[0], [0]]),
        ('OneColumnDiff', [[3], [1]]),
        ('2x2Same', [[0, 0], [0,0]]),
        ('2x2Diff', [[0, 1], [2,3]]),
        ('2x2Column', [[0,1], [0,2]]),
        ('3x3Overhang',  [[0, 0, 0], [1, 0, 2], [3, 2, 0]]),
        ('3x3Underhang',  [[3, 2, 0], [1, 0, 2], [0, 0, 0]])
    ]

    failures = 0
    bSuccessTest = True

    mp = MapParser()
    def _runTestIteration(name, input):
        print('\nRunning Test: {}'.format(name))
        bSuccessTest = True
        try:
            s = mp.arrayToSet(input)
            l = mp.setToArray(s)
            bSuccessTest = (l == input)
            if not bSuccessTest:
                print('Mismatch: InputArray {}, OutputArray {} (Set {})'.format(input, l, s))
        except:
            bSuccessTest = False
            print('EXCEPTION: {}'.format(traceback.format_exc()))
        return bSuccessTest

    for idx, (name, input) in enumerate(STANDARD_TESTS):
        failures += 1 if not _runTestIteration(name, input) else 0

    # Randomly generated tests.
    random.seed(seed)
    for idx in range(nRand):
        name = 'RandInput_{}'.format(idx)
        width = random.randint(1,10)
        height = random.randint(1,10)
        input = []
        for h in range(height):
            input.append([random.randint(0,9) for w in range(width)])
        failures += 1 if not _runTestIteration(name, input) else 0

    if failures == 0:
        print('\n\nSUCCESS!')
    else:
        print('\n\nFAILURE! ({} TESTS FAILED)'.format(failures))

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Parse block-by-block maps into a simplified format')
    parser.add_argument('-t', '--test', action='store_true', help='Run sanity tests on parser')
    DEFAULT_RAND_TESTS = 0
    parser.add_argument('-n', '--nRand', nargs='?', const=1, type=int, default=DEFAULT_RAND_TESTS, help='Number of random tests to use in unit testing. Default {}.'.format(DEFAULT_RAND_TESTS))
    parser.add_argument('-s', '--seed', type=int, help='Random seed')
    args = parser.parse_args()

    if args.test:
        runTests(args.nRand, args.seed)
