import traceback

BLOCK_SYMBOL_PREFIXES = {
    '!' :   'size',
    '.' :   'texture',
}

# List of prefixes - sorted in order of length of the prefix, to avoid shorter prefix detection (ex. '!')
# masking longer ones (ex. '!!').
SORTED_PREFIXES = []
for prefix, name in BLOCK_SYMBOL_PREFIXES.items():
    SORTED_PREFIXES.append(prefix)
SORTED_PREFIXES = sorted(SORTED_PREFIXES, key=lambda k: len(k), reverse=True)

class Symbol:
    def __init__(self, type=None, settings=None):
        self.type = type                # Type, ex. 'B' for box.
        self.settings = settings or {}  # Different settings that could apply to this type, mapped from symbol (ex. '.') to value.
        for prefix, name in BLOCK_SYMBOL_PREFIXES.items():
            self.settings.setdefault(prefix)

    def __repr__(self):
        return "%s(%r)" % (self.__class__, self.__dict__)

    def __eq__(self, other):
        return (self.type == other.type) and (self.settings == other.settings)

# Stores information about a group of blocks, i.e. a candidate for exporting as a single object.
# X positions are provided as a block offset from the leftmost column of blocks.
# Y positions are provided as a block offset from the topmost row of blocks, with more positive values being lower down.
class GroupInfo:
    def __init__(self, symbol=None, leftX=0, topY=0, dimX=0, dimY=0):
        self.symbol = symbol
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
        self.settingsMap = {}       # Dictionary mapping symbol/identifier strings, (ex. '.1', '!2') to appropriate values (ex. texture path, size, etc.)
        self.activeGroups = set()   # Groups that aren't finalized yet - could still grow.
        self.exportGroups = set()   # Finalized groups - need to be written to output.

    # Convert a text file into a condensed set of objects.
    def MapFileToSet(self, filePath):
        self.reset()
        with open(filePath, 'r') as f:
            for idx, line in enumerate(f):
                line = line.strip('\n')
                symbols = self.lineStrToSymbolList(line)
                print(f'Line {idx}, File Symbols: {symbols}')
                rowGroups = self.parseLine(symbols, topY=idx)
                self.updateGroups(rowGroups)
            # Combine any remaining active groups into the export list.
            self.exportGroups.update(self.activeGroups)
            self.activeGroups = set()

        return self.exportGroups

    # Convert a 2D array of symbols into a condensed set.
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

    # Take a set of groups and return an equivalent 2D input array.
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

    # Parse a single line of the input description, provided as a list of symbols.
    def parseLine(self, lineSymbolList, topY=0):
        rowGroups = set()
        activeSymbol = None
        runLen = 0

        if not lineSymbolList:
            return rowGroups

        for idx, symbol in enumerate(lineSymbolList):
            # Form one-dimensional groups of items within this row.
            if (activeSymbol is None) or (symbol != activeSymbol):
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

    # Turn a string of object prefix/values into a list of Symbol objects.
    def lineStrToSymbolList(self, s):
        symbols = []
        s = s.split()

        if not s:
            return symbols

        # Add any prefix definitions into stored settings.
        for prefix in SORTED_PREFIXES:
            if s[0].startswith(prefix):
                val = s[0][len(prefix):]
                self.settingsMap[prefix] = val
                print(f"Discovered prefix: '{prefix}' -> '{val}'")
                return symbols

        # Iterate backwards through possible prefixes, taking their different lengths into account.
        # On any match, store the ending value, cut it off, and continue iterating backwards.
        # This method ensures that for any prefix match, everything to the right is value, not potential other prefixes.
        for idx, sym in enumerate(s):
            #print(f'\nIdx {idx} Symbol: {sym}')
            newSymbol = Symbol()
            # Leave at least one character to hold the actual value to set for the prefix.
            endLen = 1
            while endLen < len(sym):
                #print(f'endLen {endLen}, sym {sym}')
                bMatch = False
                for prefix in SORTED_PREFIXES:
                    if sym[:-endLen].endswith(prefix):
                        newSymbol.settings[prefix] = sym[-endLen:]
                        sym = sym[:-endLen-len(prefix)]
                        bMatch = True
                        break
                if bMatch:
                    endLen = 1
                else:
                    endLen += 1
            # Type is whatever's left over after extracting prefix info.
            newSymbol.type = sym
            #print(f"Symbol '{s[idx]} -> {newSymbol}")
            symbols.append(newSymbol)

        #print(f'Symbols: {symbols}')
        return symbols
