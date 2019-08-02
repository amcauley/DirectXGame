import random
import traceback
import unittest

import MapParser

class TestGrouping(unittest.TestCase):

    def setUp(self):
        self.mp = MapParser.MapParser()

    def _runTestIteration(self, name, input):
        print('\nRunning Test: {}'.format(name))
        self.mp.reset()
        try:
            s = self.mp.arrayToSet(input)
            l = self.mp.setToArray(s)
            self.assertTrue(l == input)
        except:
            self.assertTrue(False, 'EXCEPTION: {}'.format(traceback.format_exc()))

    def test_standard(self):
        # (Name, Input 2D Array)
        # Since sets don't have an order, the output sets might change run-to-run, so can't test the set directly.
        # Instead, verify that the operation is reversible.
        # These tests use integers for symbols instead of the full symbol class for simplicity.
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

        for name, input in STANDARD_TESTS:
            self._runTestIteration(name, input)

    def test_random(self):
        # Randomly generated tests.
        RANDOM_SEED = 42
        NUM_RAND_GROUP_TESTS = 100

        random.seed(RANDOM_SEED)
        for idx in range(NUM_RAND_GROUP_TESTS):
            name = 'RandInput_{}'.format(idx)
            width = random.randint(1,10)
            height = random.randint(1,10)
            input = []
            for h in range(height):
                input.append([random.randint(0,9) for w in range(width)])
            self._runTestIteration(name, input)

            
class TestSymbols(unittest.TestCase):

    def setUp(self):
        self.mp = MapParser.MapParser()

    def test_toList(self):
        # Input string, output list
        EXPECTED_IO = [
            ('B.1', [
                MapParser.Symbol(
                    type='B',
                    settings={
                        '.': '1',}),
            ]),
            ('A.1 B.2', [
                MapParser.Symbol(
                    type='A',
                    settings={
                        '.': '1',}),
                MapParser.Symbol(
                    type='B',
                    settings={
                        '.': '2',}),
            ]),
        ]

        for i, oExpected in EXPECTED_IO:
            self.mp.reset()
            o = self.mp.lineStrToSymbolList(i)
            self.assertTrue(o == oExpected, 'Symbol mismatch: expected {}, actual {}'.format(oExpected, o))

    def test_readFromFile(self):
        print('\nReading Map From File...')
        self.mp.reset()
        s = self.mp.MapFileToSet('TestMap.txt')
        print(s)

if __name__ == '__main__':
    unittest.main()