import subprocess
import argparse
import sys

'''
TODO: add a function that automates the compile process
'''

class ArgumentHandling():
    def __init__(self):
        self.parser = argparse.ArgumentParser()

        self.parser.add_argument('-t', type=str, help='name of a test file in tests/ to run')

        self.args = vars(self.parser.parse_args())

    def get_argument_file(self):
        if(self.args['t'] != None):
            try:
                arg = str(self.args['t'])
                return arg
            except Exception as e:
                return e
        else:
            return None



def read_test(filename):
    with open(filename, 'r') as f:
        test_input = f.read()

    return test_input

def turn_raw_input_into_clean(raw_input):
    return raw_input

def test(output, expected):
    print(output == expected)

if __name__ == "__main__":
    arg_handle = ArgumentHandling()

    # check for argument
    if(arg_handle.get_argument_file() != None):
        # case when only one inputed file is tested
        file_path = 'tests/' + str(arg_handle.get_argument_file())
    else:
        # case when every file from 'tests/' folder is tested
        pass
    

    run_exe = ['./db']

    p = subprocess.Popen(run_exe, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)


    _input = turn_raw_input_into_clean(read_test('tests/test1.txt'))

    raw_output = p.communicate(input=_input.encode())[0].decode('utf-8').replace("db > ","")

    clean_output = raw_output.splitlines()

    # not printing the last element because it is a known error `error reading input`
    print(clean_output[:-1])