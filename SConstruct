env = Environment()
source_files = Glob('src/*.c*', 'inc/*.h*')
env.Program(target='bin/mna',
            source=source_files,
            LIBS='ncurses')

# $ g++ tests/simple.cpp -lgtest -lgtest_main -o bin/test_mna
test_files = Glob('tests/*.c*', 'inc/*.h*')
env.Program(target='bin/test_mna',
            source=test_files,
            LIBS=['gtest', 'gtest_main', 'pthread'])
