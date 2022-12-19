#!/bin/bash

init()
{
	legacy_compiler=$1
	ruc_vm=$2
	test_dir=./tests 

	wait_for=10

	pass=0
	fail=0
	timeout=0

	equals=0

	ulimit -s 81920
}

build()
{
	if [[ $legacy_compiler = "--build" ]] ; then
		git fetch origin legacy:legacy
		sh ./scripts/build_legacy.sh
		legacy_compiler=../compiler/ruc
		ruc_vm=../vm/ruc-vm
	fi

	cd src
	`$legacy_compiler main.ruc>/dev/null 2>/dev/null`
	if ! [[ $? -eq 0 ]]; then
		exit 21
	fi
	cd ..
}

test()
{
	cd $test_dir
	cp ../src/keywords.txt keywords.txt
	for code in ./*.c ./*/*.c ./*/*/*.c
	do
		if [[ $code == ./_* ]] ; then
			continue
		fi
		cp $code main.ruc

		$legacy_compiler main.ruc >/dev/null 2>/dev/null
		mv export.txt $code.export.legacy >/dev/null 2>/dev/null
		if [[ $? != '0' ]]; then
			continue
		fi

		out=`timeout $wait_for $ruc_vm ../src/export.txt >$code.out 2>$code.out`

		case $? in
			0)
				echo -e "\x1B[1;32m build passing \x1B[1;39m: $code"
				mv export.txt $code.export
				let pass++
				;;
			124)
				echo -e "\x1B[1;34m build timeout \x1B[1;39m: $code"
				let timeout++
				;;
			*)
				echo -e "\x1B[1;31m build failing \x1B[1;39m: $code"
				let fail++
				;;
		esac

		if cmp -s "$code.export.legacy" "$code.export"; then
			echo -e "\x1B[1;32m export.txt same as from legacy compiler \x1B[1;39m: $code"
			let equals++
		else 
			echo -e "\x1B[1;31m export.txt differ from legacy compiler \x1B[1;39m:  $code"
		fi

	done


	if ! [[ -z $full_out ]] ; then
		echo
	fi

	echo -e "\x1B[1;39m pass = $pass, fail = $fail, timeout = $timeout, equals = $equals"
}

main()
{
	cd `dirname $0`/..
	init $@

	build
	test

	if [[ $fail != 0 || $timeout != 0 ]] ; then
		exit 1
	fi

	exit 0
}

main $@
