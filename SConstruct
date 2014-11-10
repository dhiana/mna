env = Environment()
env.Program(target='bin/mna',
            source=["src/mna.c"],
            LIBS='ncurses')

# $ g++ -I$GTEST_ROOT/include tests/simple.cpp $GTEST_ROOT/libgtest.a $GTEST_ROOT/libgtest_main.a -lpthread -o bin/test_mna
env.Prepend(CPPPATH = ['$GTEST_ROOT',
                       '$GTEST_ROOT/include'])
gtest = 'libgtest.a'
gtest_main = 'libgtest_main.a'
gtest_libs = [gtest, gtest_main, 'pthread']
env.Program(target='bin/test_mna_local',
            source=["tests/simple.cpp"],
            LIBS=gtest_libs)

# $ g++ tests/simple.cpp -lgtest -lgtest_main -o bin/test_mna
env.Program(target='bin/test_mna',
            source=["tests/simple.cpp"],
            LIBS=['gtest', 'gtest_main', 'pthread'])
