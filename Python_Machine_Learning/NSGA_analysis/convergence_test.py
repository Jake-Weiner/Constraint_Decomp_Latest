
import matplotlib.pyplot as plt

def main():

    global_decomp_dict = dict()

    input_file = "/home/jake/PhD/Decomposition/Output/testing/NSGA_Tests/cost266-UUE.mps_pareto_opt_sols"
    seek_input_file = "/home/jake/PhD/seek_test_python"

    #test seek

    # with open(seek_input_file, "r") as seek_input_test_fs:
    #     next(seek_input_test_fs)
    #     for line in seek_input_test_fs:
    #         print(line)

    #initalise first generation solutions
    with open(input_file,"r") as input_fs:
        for generation in input_fs:
            #split up line based on whitespace
            decomp_list = generation.split()
            for decomp in decomp_list:
                #remove trailing comma
                decomp = decomp[:-1]
                decomp_split = decomp.split(",")
                decomp_tuple = tuple(decomp_split)
                global_decomp_dict[decomp_tuple] = True
                break

    decomp_props = []
    pareto_sizes = []
    #starting from 2nd generation
    with open(input_file, "r") as input_fs:
        next(input_fs)
        for generation in input_fs:
            count_decomp_in_gen_next = 0
            total_decomp_size = len(global_decomp_dict)
            # split up line based on whitespace
            decomp_list = generation.split()
            for decomp in decomp_list:
                # remove trailing comma
                decomp = decomp[:-1]
                decomp_split = decomp.split(",")
                decomp_tuple = tuple(decomp_split)
                #check to see if decomp is stored within dict
                if decomp_tuple in global_decomp_dict:
                    count_decomp_in_gen_next += 1
                    # global_decomp_dict[decomp_tuple] = True
            decomp_props.append(count_decomp_in_gen_next / total_decomp_size)
            global_decomp_dict.clear()
            # repopulate dict new dict
            for decomp in decomp_list:
                # remove trailing comma
                decomp = decomp[:-1]
                decomp_split = decomp.split(",")
                decomp_tuple = tuple(decomp_split)
                global_decomp_dict[decomp_tuple] = True
            pareto_sizes.append(len(decomp_list))

    print(decomp_props)
    print(len(decomp_props))
    x_vals = [i for i in range(0,len(decomp_props))]
    print(x_vals)
    print(pareto_sizes)

    plt.figure(figsize=(14, 6))
    plt.subplot(121)
    plt.scatter(x_vals,decomp_props, s=6)
    plt.xlabel("Generation Number")
    plt.ylabel("Percentage of gen n-1 pareto solutions in gen n")

    plt.subplot(122)
    plt.scatter(x_vals,pareto_sizes, s=6)
    plt.xlabel("Generation Number")
    plt.ylabel("Number of pareto optimal solutions")

    plt.tight_layout(pad=3.0)
    plt.savefig("/home/jake/PhD/Decomposition/Output/testing/NSGA_Tests/pareto_plot")

if __name__ == "__main__":

    main()