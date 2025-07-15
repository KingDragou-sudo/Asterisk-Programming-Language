# Asterisk Programming Language

Alright, so I'll just get the CURRENT documentation outta the way.

# Data Types:
-    integers
-    floats
-    strings
-    chars
-    bools
-    ROOMs (replaces arrays and eventually vectors or maps)


# Setting a variable:

    var name = value; (type is always inferred)
    var name; (this'll default to 0)

# Binary Operators:
    +
    -
    *
    /
    ^

# Using ROOMs

    how to use: variablename_ROOM

    var name_ROOM = [value, value, value]; (type is always inferred)
    var name_ROOM; (this'll default to an empty ROOM)

    print(name_ROOM[0]); (prints the first value)
    print(name_ROOM[0] == name_ROOM[1]); (prints true or false)

    name_ROOM[0] = value; (changes the first value)

    var name2_ROOM = name_ROOM; (copies the ROOM)

# Statements:

    if (condition) then {
        blah blah blah whatever
    }
    else {
        blahhhhhhhh
    }


    while (condition){
        massive blah
    }

    for(var i = 0; i < 10; i++){
        blah blah blah
    }

# Keywords:
    var
    func
    if
    then
    ret
    while
    for
    else
    continue
    break
    in
    room
    print
    round
    floor
    ceil
    abs
    min
    max
    sqrt
    pow
