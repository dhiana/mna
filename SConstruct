env = Environment(CPPPATH='include')
source_files = Glob('src/*.c*', 'include/*.h*')
source_files.extend(
    Glob('src/matrix/*.c*', 'include/matrix/*.h*')
)
env.Program(target='bin/mna',
            source=source_files,
            LIBS='ncurses')

# $ g++ tests/simple.cpp -lgtest -lgtest_main -o bin/test_mna
test_files = Glob('tests/*.c*')
test_files.extend(
    Glob('src/matrix/*.c*', 'include/matrix/*.h*')
)
env.Program(target='bin/test',
            source=test_files,
            LIBS=['gtest', 'gtest_main', 'pthread'])
