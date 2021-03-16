project('snatcher', 'cpp',
    default_options: [
        'buildtype=release',
        'default_library=static',
        'b_ndebug=if-release',
        'b_lto=true',
        'strip=true',
        'warning_level=2',
        'c_std=gnu11',
        'cpp_std=gnu++2a',
    ],
)

deps = []
deps += dependency('qt5', modules: ['Core', 'Gui', 'Widgets'])
deps += dependency('boost')
deps += dependency('openssl')
deps += dependency('threads')

qt5 = import('qt5')
moc_files = qt5.preprocess(
    moc_headers: 'src/mainwindow.hpp',
    ui_files:    'src/mainwindow.ui',
    qresources:  'resources/resources.qrc',
    dependencies: deps,
)

exe_src = files(
    'src/main.cpp',
    'src/mainwindow.cpp',
)

executable('snatcher', exe_src,
    moc_files,
    dependencies : deps
)
