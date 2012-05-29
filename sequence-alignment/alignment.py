#!/usr/bin/env python
import shlex
import sys

# Check if a string is an int
def is_int(s):
    try:
        int(s)
        return True
    except ValueError:
        return False

# Obtains the scoring matrix for a given file
def get_R(file):
    f = open(file, 'r')

    amin = shlex.split(f.readline())

    matrix = {}

    i = 0
    for eachLine in f:
        j = 0
        for x in (shlex.split(eachLine)[1:]):
            cur = int(x)
            matrix[(amin[i], amin[j])] = cur
            matrix[(amin[j], amin[i])] = cur
            j = j+1
        i = i + 1
    f.close()

    return matrix

# Gets H matrix
def get_H(R, g0, ge, q, d):
    m = len(q)+1
    n = len(d)+1
    lhighest = []
    maxvalue = 0

    # initialization to 0
    H = [[[0 for Hs in range(5)] for col in range(n)] for row in range(m)]
    pointer = [[0 for col in range(n)] for row in range(m)]

    # dynamic programming loop
    for i in range(1,m):
        for j in range(1,n):
            H[i][j][1] = max(H[i-1][j][1] - ge,
                    H[i-1][j][2] - (g0+ge),
                    H[i-1][j][3] - (g0+ge))

            H[i][j][2] = max(H[i][j-1][2] - ge,
                    H[i][j-1][1] - (g0+ge),
                    H[i][j-1][3] - (g0+ge))

            H[i][j][3] = H[i-1][j-1][0] + R[q[i-1], d[j-1]]

            H[i][j][0] = max(0, H[i][j][1], H[i][j][2], H[i][j][3])

            # we save the maximum value of the matrix
            if (H[i][j][0] > maxvalue):
                maxvalue = H[i][j][0]

            # we store the arrow in pointer matrix
            # directions:
            #   - UP: 1
            #   - LEFT: 2
            #   - DIAGONAL: 3
            #   - END: 0
            if H[i][j][0] == 0:
                pointer[i][j] = 0
            elif H[i][j][0] == H[i][j][1]:
                pointer[i][j] = 1
            elif H[i][j][0] == H[i][j][2]:
                pointer[i][j] = 2
            elif H[i][j][0] == H[i][j][3]:
                pointer[i][j] = 3
    return H, pointer, maxvalue

# Makes the backtracking from a list of positions with
# the highest values, changes H with the paths starting
# in that highest value and returns a list with the
# alignments found in string form
def backtrack(H, pointer, lhighest, q, d):
    lalignments = []
    for item in lhighest:
        alignQ = ''
        alignD = ''
        i = item[0]
        j = item[1]
        H[i][j][4] = 1
        while pointer[i][j] != 0:
            if pointer[i][j] == 3:
                # if this is 1 it's part of the path
                H[i-1][j-1][4] = 1
                alignQ = alignQ + q[i-1]
                alignD = alignD + d[j-1]
                i = i-1
                j = j-1
            elif pointer[i][j] == 2:
                H[i][j-1][4] = 1
                alignQ = alignQ + '-'
                alignD = alignD + d[j-1]
                j = j-1
            elif pointer[i][j] == 1:
                H[i-1][j][4] = 1
                alignQ = alignQ + q[i-1]
                alignD = alignD + '-'
                i = i-1
        # reverse results
        alignQ = alignQ[::-1]
        alignD = alignD[::-1]
        lalignments.append([alignQ, alignD])
    return lalignments

# Finds a list of the positions where the highest values are
def find_highest(H, maxvalue):
    m = len(H)
    n = len(H[0])
    highest = 0

    lhighest = []

    for i in range(m):
        for j in range(n):
            if H[i][j][0] == maxvalue:
                lhighest.append([i, j])
    return lhighest


# Pretty-print of H matrix
def print_H(H, q, d):
    print("     ", end="")
    for i in range(len(d)+1):
        print (format(i, ">5d"), end=" ")
    print("\n")

    print("               ", end="")
    for i in range(len(d)):
        print (d[i], end="     ")
    print("\n")
    for i in range(0,len(H)):
        if i>0:
            print(format(i, ">2d"), end=" ")
            print(q[i-1], end=" ")
        else:
            print(" 0 ",end="  ")
        for j in range(0,len(H[i])):
            if H[i][j][4]:
                print(format(H[i][j][0], ">4d"), end="")
                print("*", end=" ")
            else:
                print(format(H[i][j][0], ">5d"), end=" ")
        print()
    print()

def usage():
    print ("Usage: ./alignment file g0 ge q d\n\tfile: scoring matrix file\n\tg0: gap start\n\tge: gap extension\n\tq: sequence 1\n\td: sequence 2")

def main():
    try:
        if len(sys.argv) != 6:
            raise ValueError
        f = sys.argv[1]
        g0 = int(sys.argv[2])
        ge = int(sys.argv[3])
        q = sys.argv[4]
        d = sys.argv[5]

        # conversion to uppercase
        q = q.upper()
        d = d.upper()

        R = get_R(f)
        H, pointer, maxvalue = get_H(R, g0, ge, q, d)

        lhighest = find_highest(H, maxvalue)

        lalignments = backtrack(H, pointer, lhighest, q, d)

        print_H(H,q,d)

        for item in lalignments:
            print(item[0])
            print(item[1])
            print()
        print()
    except IOError:
        print ("I/O error: Cannot open", sys.argv[1])
    except ValueError:
        usage()
    except KeyError:
        print ("Invalid letters in aminoacid sequences")
    except:
        print("Unexpected error:", sys.exc_info()[0])
        raise

if __name__ == "__main__":
    main()
