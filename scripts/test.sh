#!/bin/bash

init()
{
	output_time=0.1
	wait_for=2
	vm_release=master

	while ! [[ -z $1 ]]
	do
		case "$1" in
			-h|--help)
				echo -e "Usage: ./${0##*/} [KEY]..."
				echo -e "Keys:"
				echo -e "\t-h, --help\tTo output help info"
				echo -e "\t-s, --silence\tFor silence testing"
				echo -e "\t-d, --debug\tSwitch on debug tracing"
				echo -e "\t-v, --virtual\tSet RuC virtual machine release"
				echo -e "\t-o, --output\tSet output printing time (default = 0.1)"
				echo -e "\t-w, --wait\tSet waiting time for timeout result (default = 2)"
				exit 0
				;;
			-s|--silence)
				silence=$1
				;;
			-d|--debug)
				debug=$1
				;;
			-v|--virtual)
				vm_release=$2
				shift
				;;
			-o|--output)
				output_time=$2
				shift
				;;
			-w|--wait)
				wait_for=$2
				shift
				;;
		esac
		shift
	done

	success=0
	failure=0
	timeout=0
}

build_vm()
{
	rm -rf ruc-vm
	git clone -b $vm_release --recursive https://github.com/andrey-terekhov/RuC-VM ruc-vm

	cd ruc-vm
	mkdir -p build && cd build && cmake ..
	if ! cmake --build . --config Release ; then
		exit 1
	fi

	cd ../..
	ruc_interpreter=./ruc-vm/build/ruc-vm
}

build()
{
	cd `dirname $0`/..
	mkdir -p build && cd build && cmake ..
	if ! cmake --build . --config Release ; then
		exit 1
	fi

	ruc_compiler=./ruc

	build_vm

	test_dir=../tests
	error_dir=../tests/errors
	exec_dir=../tests/executable
	
	error_subdir=errors
}

internal_timeout()
{
	which timeout
	if [[ $? == 0 ]] ; then
		timeout $@
	else
		perl -e 'alarm shift; exec @ARGV' $@;
	fi
}

message_success()
{
	if [[ -z $debug ]] ; then
		if [[ -z $silence ]] ; then
			echo -e "\x1B[1;32m $action success \x1B[1;39m: $code"
			sleep $output_time
		fi
	fi
}

message_timeout()
{
	if [[ -z $silence ]] ; then
		echo -e "\x1B[1;34m $action timeout \x1B[1;39m: $code"
		sleep $output_time
	fi
}

message_failure()
{
	if [[ -z $silence ]] ; then
		echo -e "\x1B[1;31m $action failure \x1B[1;39m: $code"
		sleep $output_time
	fi
}

execution()
{
	if [[ $code == $exec_dir/* ]] ; then
		action="execution"
		internal_timeout $wait_for $ruc_interpreter export.txt >/dev/null 2>/dev/null

		case "$?" in
			0)
				if [[ $code == */$error_subdir/* ]] ; then
					message_failure
					let failure++
				else
					message_success
					let success++
				fi
				;;
			124|142)
				message_timeout
				let timeout++
				;;
			*)

				if [[ $code == */$error_subdir/* ]] ; then
					message_success
					let success++
				else
					message_failure
					let failure++

					if ! [[ -z $debug ]] ; then
						$ruc_interpreter export.txt
					fi
				fi
				;;
		esac
	fi
}

test()
{
	# Do not use names with spaces!
	for code in `find ${test_dir} -name *.c`
	do
		action="compiling"
		internal_timeout $wait_for $ruc_compiler $code >/dev/null 2>/dev/null

		case "$?" in
			0)
				if [[ $code == $error_dir/* ]] ; then
					message_failure
					let failure++
				else
					message_success
					execution
				fi
				;;
			124|142)
				message_timeout
				let timeout++
				;;
			*)
				if [[ $code == $error_dir/* ]] ; then
					message_success
					let success++
				else
					message_failure
					let failure++

					if ! [[ -z $debug ]] ; then
						$ruc_compiler $code
					fi
				fi
				;;
		esac
	done


	if [[ -z $silence ]] ; then
		echo
	fi

	echo -e "\x1B[1;39m success = $success, failure = $failure, timeout = $timeout"
}

main()
{
	init $@

	build
	test

	if [[ $failure != 0 || $timeout != 0 ]] ; then
		exit 1
	fi

	exit 0
}

main $@
