TESTS = []


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



#------------------------------------------------------------------------------------
# TEST 7 (testing `.btree` meta command in a 3-node tree (internal root, and 2 leaf)|
#------------------------------------------------------------------------------------
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