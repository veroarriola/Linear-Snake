#!/bin/bash
# Get directory of this script
script_dir=$( dirname "${BASH_SOURCE[0]}" )
results_dir="data"

# Generate histories for all files in dataset
dataset_dir=/home/blackzafiro/Programas/PhD/MassSpringIV/data/press

sponge_data=(sponge_centre_100 sponge_longside_100 sponge_shortside_100)
plasticine_data=(plasticine_centre_100_below plasticine_longside_100_below plasticine_shortside_100_below)
#sponge_data=(sponge_shortside_100)
#plasticine_data=()

declare -A materials
materials=([sponge]=${sponge_data[*]} [plasticine]=${plasticine_data[*]})

# Create directory for results
mkdir -p $results_dir
for material in ${!materials[@]}
do
	echo $material
	echo ${materials[$material]}
	for bundle_name in ${materials[$material]}
	do
		base_name=$dataset_dir/$bundle_name
		command="$script_dir/simple_tracker $base_name $material $results_dir/${bundle_name}_history.hist"
		echo $command
		$command
	done
done

