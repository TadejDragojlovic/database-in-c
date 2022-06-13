import subprocess
import os
import colorama

import tests

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
        print(colorama.Fore.RED + "TEST FAILED." + colorama.Fore.RESET)


if __name__ == "__main__":
    compile()
    run_exe = './db'

    # RUNNING TESTS
    for i in range(len(tests.TESTS)):
        test_output = test_driver(tests.TESTS[i][0])
        test_expectation = tests.TESTS[i][1]

        # print(test_output)
        # print(test_expectation)
        
        test_evaluation(test_output, test_expectation, i)

    cleanup()