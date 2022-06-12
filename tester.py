import subprocess
import os
import colorama

'''
TODO: Add further checking if '-t' input file is correct and exists
'''

def compile():
    '''
    compile the program with makefile
    '''
    os.system('make compile')

def cleanup():
    '''
    remove the exe of the program after completion
    '''
    os.system('make clean')

def test_driver(test_input) -> list:
    '''
    driver function for testing, returns raw output of a single test
    [str]
    '''
    p = subprocess.Popen(run_exe, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

    cmd = '\n'.join(test_input)
    cmd+='\n' # adding this just to fix a bug where last character is ommited

    # deleting every 'db > ' just to keep things more clean
    raw_output = p.communicate(input=cmd.encode())[0].decode('utf-8').replace("db > ","")

    clean_output = raw_output.splitlines()

    # last input is an empty row, so output for it is 'Error reading output'
    return clean_output[:-1]


def test_evaluation(output, expected, test_number):
    '''
    main function that runs the test
    [void]
    '''
    if(output == expected):
        print(colorama.Fore.GREEN + f"TEST NUMBER ({test_number+1}) SUCCESSFULLY PASSED." + colorama.Fore.RESET)
    else:
        print(colorama.Fore.RED + "TEST FAILED.")


if __name__ == "__main__":
    compile()
    run_exe = './db'

    tests_to_run = []
    tests_expectations = []

    # TEST 1
    _input = ['insert 1 cstack foo@gmail.com', 'select']
    to_expect = ['Inserted.', '(1, cstack, foo@gmail.com)']

    tests_to_run.append(_input)
    tests_expectations.append(to_expect)

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

    tests_to_run.append(_input)
    tests_expectations.append(to_expect)


    # TEST 3 (testing table row limit)
    _input = []
    to_expect = []

    n = 1401
    for i in range(1, n+1):
        _input.append(f'insert {i} user{i} email{i}@gmail.com')

    to_expect = ['Inserted.' for x in range(n-1)]
    to_expect.append('ERROR. Table is full.')

    tests_to_run.append(_input)
    tests_expectations.append(to_expect)


    # RUNNING TESTS
    for i in range(len(tests_to_run)):
        test_output = test_driver(tests_to_run[i])
        test_expectation = tests_expectations[i]

        # print(test_output)
        # print(test_expectation)
        
        test_evaluation(test_output, test_expectation, i)

    cleanup()