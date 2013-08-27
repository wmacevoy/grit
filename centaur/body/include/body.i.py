def p(arg):
    print arg

def i(arg):
    for line in open(arg,'r'):
        line=line.rstrip()
        if line.strip() != '#pragma once':
            print line

p('%module body')
p('')
p('%{')
p('#define SWIG_FILE_WITH_INIT')
i('Body.h')
p('%}')
p('')
i('Body.h')
