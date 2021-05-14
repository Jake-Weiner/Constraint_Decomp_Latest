

# arr = array to sort
# n is the length of the array
def recursive_insertion_sort(arr,  n):
    # // Base case
    if (n <= 1):
        return
    # // Sort first n-1 elements

    recursive_insertion_sort(arr, n-1)
    val = arr[n-1]
    pos = n-2 # index of the element before (n-1 - 1)

    #find the position for the char to fit in. Pos - 1 is starting from 2 in.
    while (pos >= 0 and arr[pos-1] != val):
        arr[pos+1] = arr[pos] # swap
        pos = pos - 1
    arr[pos+1] = val

def main():

    test_array = ["M","F","F","M","F","F","F","M","F", "M"]

    recursive_insertion_sort(test_array,len(test_array))

    print(test_array)
    print("hello")


if __name__ == main():

    main()