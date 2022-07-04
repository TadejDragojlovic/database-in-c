import random

TESTS = []

'''
All tests are done on:
    - TABLE_MAX_PAGES: 300000
    - INTERNAL_NODE_MAX_CELLS: 510
'''

#---------
# TEST 1 |
#---------
test_name = 'basic, 1 insertion'
_input = ['insert 1 cstack foo@gmail.com', 'select']
_expect = ['Inserted.', '(1, cstack, foo@gmail.com)']

TESTS.append({'name': test_name, 'inputs': [_input], 'expectations': [_expect]})


#-------------------------------------------------------------
# TEST 2 (more complex, automating the making of a test case)|
#-------------------------------------------------------------
test_name = 'multiple insertions (4)'
# REMINDER: if 'select' is executed and the table is empty, it won't output anything
_input = ['select']

# list starts of empty, because select doesn't output anything on an empty table
_expect = []

n_of_inserts = 4

# adding to _input and _expect simultaneously to save time
for i in range(n_of_inserts):
    _expect.append('Inserted.')
for i in range(1, n_of_inserts+1):
    _input.append(f'insert {i} test{i} foo{i}@gmail.com')
    _expect.append(f'({i}, test{i}, foo{i}@gmail.com)')
_input.append('select')

TESTS.append({'name': test_name, 'inputs': [_input], 'expectations': [_expect]})



#-----------------------------------------------------
# TEST 3 (testing for buffer overflow when inserting)|
#-----------------------------------------------------
test_name = 'faulty input, buffer overflow prevention'
_input = [f"insert 1 {'a'*33} {'b'*256}", "select"]
_expect = ["String inserted is too long."]

TESTS.append({'name': test_name, 'inputs': [_input], 'expectations': [_expect]})



#--------------------------------------------------
# TEST 4 (testing for negative ids when inserting)|
#--------------------------------------------------
test_name = 'negative ids'
_input = ["insert -1 some_username foo@gmail.com"]
_expect = ["Negative ID inserted. ID must be a positive integer."]

TESTS.append({'name': test_name, 'inputs': [_input], 'expectations': [_expect]})



#------------------------------------------------
# TEST 5 (testing if file is saved successfully)|
#------------------------------------------------
test_name = 'file save'
_input = []
_expect = []

n = 20
for i in range(1, n+1):
    _input.append(f'insert {i} user{i} email{i}@gmail.com')

_expect = ['Inserted.' for x in range(n)]
_input.append('.exit')

_input1 = ["select"]
_expect1 = []
for i in range(1, n+1):
    _expect1.append(f'({i}, user{i}, email{i}@gmail.com)')

TESTS.append({'name': test_name, 'inputs': [_input, _input1], 'expectations': [_expect, _expect1]})



#--------------------------------------------
# TEST 6 (testing `.constants` meta command)|
#--------------------------------------------
test_name = '`.constants` meta command'
_input = []
_expect = []

_input.append('.constants')

_expect.append('Constants:')
constants = '''ROW_SIZE: 293
COMMON_NODE_HEADER_SIZE: 6
LEAF_NODE_HEADER_SIZE: 14
LEAF_NODE_CELL_SIZE: 297
LEAF_NODE_SPACE_FOR_CELLS: 4082
LEAF_NODE_MAX_CELLS: 13'''.split('\n')

for line in constants:
    _expect.append(line)

TESTS.append({'name': test_name, 'inputs': [_input], 'expectations': [_expect]})



#-------------------------------------------------------------------------------------
# TEST 7 (testing `.btree` meta command in a 3-node tree (internal root, and 2 leaf))|
#-------------------------------------------------------------------------------------
test_name = '`.btree` of a 3-node tree'
_input = []
_expect = []

n = 21
for i in range(1, n+1):
    _input.append(f"insert {i} user{i} email{i}@gmail.com")
_input.append('.btree')

_expect = ['Inserted.' for x in range(n)]
     
_expect.append('Btree:')
btree_output = '''- internal (size 2)
  - leaf (size 7)
    - 1
    - 2
    - 3
    - 4
    - 5
    - 6
    - 7
  - key 7
  - leaf (size 7)
    - 8
    - 9
    - 10
    - 11
    - 12
    - 13
    - 14
  - key 14
  - leaf (size 7)
    - 15
    - 16
    - 17
    - 18
    - 19
    - 20
    - 21'''.split('\n')
for line in btree_output:
    _expect.append(line)

TESTS.append({'name': test_name, 'inputs': [_input], 'expectations': [_expect]})



#----------------------------------------------------------------------
# TEST 8 (testing insertion already inserted keys, duplicate id check)|
#----------------------------------------------------------------------
test_name = 'duplicate key/id'
_input = []
_expect = []

n = 32
for i in range(1, n+1):
    _input.append(f'insert {i} user{i} email{i}@gmail.com')
_input.append('insert 12 user12 email12@gmail.com')
_input.append('insert 31 user12 email12@gmail.com')
_input.append('insert 1 user1 email1@gmail.com')
_input.append('insert 32 user32 email32@gmail.com')

_expect = ['Inserted.' for x in range(n)]
for i in range(4):
    _expect.append('Error: Inserted id already exists in the table.')

TESTS.append({'name': test_name, 'inputs': [_input], 'expectations': [_expect]})



#-----------------------------------------------------------------------------------------------------
# TEST 9 (testing 2-internal node layer limit, maxing out leaf nodes, saving it into a file, ~1.07gb)|
#-----------------------------------------------------------------------------------------------------
'''
* Multiple for loops needed to insert in a certain order,
* if we just try to insert 1827847 rows, we will eventually hit the limit at `920000`
    -> the reason for this is because the tree is self-balancing, so all internal nodes will have around 256 leaf nodes,
       only the rightmost child will have 511 leaf nodes
    -> this happens because we are inserting everything in the order (and each new row id is incremented by 1)
'''
test_name = 'max out number of leaf nodes in the table (2-internal node layers)'
_input = []
_expect = []

n = 1827847

# part 1
count = 0
j = 0
while j<n+1:
    if (count > 0 and j%7 == 0):
        j += 8
        count = 0
    else:
        j += 1
        count += 1
    _input.append(f'insert {j} u{j} e{j}')
    _expect.append('Inserted.')

# part 2
x = 9
while x < n+1:
    for i in range(x, x+6):
        _input.append(f'insert {i} u{i} e{i}')
        _expect.append('Inserted.')
    x+=14

# part 3
start = 8
count = 0
for i in range(start, n+1, 14):
    if count == 255:
        count = 0
        continue
    _input.append(f'insert {i} u{i} e{i}')
    _expect.append('Inserted.')
    count += 1

_input.append('.exit')

TESTS.append({'name': test_name, 'inputs': [_input], 'expectations': [_expect]})