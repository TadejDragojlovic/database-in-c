TESTS = []


#---------
# TEST 1 |
#---------
_input = ['insert 1 cstack foo@gmail.com', 'select']
_expect = ['Inserted.', '(1, cstack, foo@gmail.com)']

TESTS.append({'inputs': [_input], 'expectations': [_expect]})


#-------------------------------------------------------------
# TEST 2 (more complex, automating the making of a test case)|
#-------------------------------------------------------------
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

TESTS.append({'inputs': [_input], 'expectations': [_expect]})



#-----------------------------------------------------
# TEST 3 (testing for buffer overflow when inserting)|
#-----------------------------------------------------
_input = [f"insert 1 {'a'*33} {'b'*256}", "select"]
_expect = ["String inserted is too long."]

TESTS.append({'inputs': [_input], 'expectations': [_expect]})



#--------------------------------------------------
# TEST 4 (testing for negative ids when inserting)|
#--------------------------------------------------
_input = ["insert -1 some_username foo@gmail.com"]
_expect = ["Negative ID inserted. ID must be a positive integer."]

TESTS.append({'inputs': [_input], 'expectations': [_expect]})



#------------------------------------------------
# TEST 5 (testing if file is saved successfully)|
#------------------------------------------------
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

TESTS.append({'inputs': [_input, _input1], 'expectations': [_expect, _expect1]})


# #--------------------------------
# # TEST 6 (saving data to a file)|
# #--------------------------------
# # MULTIPART TEST (runs './db' more than once)
# # Tests if data is written on a file correctly and saved
# _input = ["insert 1 cstack foo@gmail.com", ".exit"]
# _expect = ["Inserted."]

# _input1 = ["select"]
# _expect1 = ["(1, cstack, foo@gmail.com)"]

# TESTS.append({'inputs': [_input, _input1], 'expectations': [_expect, _expect1]})