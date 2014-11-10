env = Environment()
env.Program(target='bin/mna',
            source=["src/mna.c"],
            LIBS='ncurses')

# $ g++ tests/simple.cpp -lgtest -lgtest_main -o bin/test_mna
env.Program(target='bin/test_mna',
            source=["tests/simple.cpp"],
            LIBS=['gtest', 'gtest_main', 'pthread'])
