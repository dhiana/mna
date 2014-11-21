env = Environment(CPPPATH='include')

# Add new directory here!!!
globs = [
    Glob('src/matrix/*.c*', 'include/matrix/*.h*'),
    Glob('src/circuits/*.c*', 'include/circuits/*.h*'),
]


source_files = Glob('src/*.c*', 'include/*.h*')
[source_files.extend(glob) for glob in globs]
env.Program(target='bin/mna',
            source=source_files,
            LIBS='ncurses')

test_files = Glob('tests/*.c*')
[test_files.extend(glob) for glob in globs]
env.Program(target='bin/test',
            source=test_files,
            LIBS=['gtest', 'gtest_main', 'pthread'])
