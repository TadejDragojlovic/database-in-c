
TESTS = []


# TEST 1
_input = ['insert 1 cstack foo@gmail.com', 'select']
to_expect = ['Inserted.', '(1, cstack, foo@gmail.com)']

TESTS.append([_input, to_expect])

# TEST 2 (more complex, automating the making of a test case)
# REMINDER: if 'select' is executed and the table is empty, it won't output anything
_input = ['select']

# list starts of empty, because select doesn't output anything on an empty table
to_expect = []

n_of_inserts = 4

# adding to _input and to_expect simultaneously to save time
for i in range(n_of_inserts):
    to_expect.append('Inserted.')
for i in range(1, n_of_inserts+1):
    _input.append(f'insert {i} test{i} foo{i}@gmail.com')
    to_expect.append(f'({i}, test{i}, foo{i}@gmail.com)')
_input.append('select')

TESTS.append([_input, to_expect])


# TEST 3 (testing table row limit)
_input = []
to_expect = []

n = 1301
for i in range(1, n+1):
    _input.append(f'insert {i} user{i} email{i}@gmail.com')

to_expect = ['Inserted.' for x in range(n-1)]
to_expect.append('ERROR. Table is full.')

TESTS.append([_input, to_expect])


# TEST 4 (testing for buffer overflow when inserting)
_input = [f"insert 1 {'a'*33} {'b'*256}", "select"]
to_expect = ["String inserted is too long."]

TESTS.append([_input, to_expect])


# TEST 5 (testing for negative ids when inserting)
_input = ["insert -1 some_username foo@gmail.com"]
to_expect = ["Negative ID inserted. ID must be a positive integer."]

TESTS.append([_input, to_expect])