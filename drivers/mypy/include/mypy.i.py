def p(arg):
    print arg
def i(arg):
    for line in open(arg,'r'):
        line=line.rstrip()
        if line.strip() != '#pragma once':
            print line

p('%module mypy')
p('')
p('%{')
i('MyClass.h')
p('%}')
p('')
i('MyClass.h')
