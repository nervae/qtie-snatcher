project('snatcher', 'cpp')

qt5 = import('qt5')

qt5_dep = dependency('qt5', modules: ['Core', 'Gui', 'Widgets'])

moc_files = qt5.preprocess(
    moc_headers: 'src/mainwindow.hpp',
    ui_files:    'src/mainwindow.ui',
    qresources:  'resources/resources.qrc',
    dependencies: qt5_dep
)

executable('snatcher', 'src/main.cpp', 'src/mainwindow.cpp', moc_files,
    dependencies : qt5_dep)
