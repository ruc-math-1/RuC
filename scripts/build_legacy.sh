build_compiler()
{
    cd compiler
    git checkout legacy
    cmake .
    cmake --build . --config Release
    cd ..
}

build_vm()
{
    cd vm
    git checkout legacy
    cmake .
    cmake --build . --config Release
    cd ..
}

main()
{
    cd `dirname $0`/..
    
    if ! git clone https://github.com/ruc-math-1/RuC-VM vm ; then
        exit 11
    fi
    git clone . compiler

    
    if ! build_compiler ; then
        exit 12
    fi

    
    if ! build_vm ; then
        exit 13
    fi

    exit 0
}

main