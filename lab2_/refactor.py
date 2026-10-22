import sys

with open('sort_compare.cpp', 'r') as f:
    lines = f.readlines()

new_lines = []
skip = False
for i, line in enumerate(lines):
    # lines are 0-indexed
    # 76-79 (77-80 1-based)
    if i == 79: # using namespace std; // correct accordingly
        new_lines.append("#include <sys/stat.h>\n")
        new_lines.append("#include <sys/types.h>\n\n")
        new_lines.append("#include \"bubble_sort.h\"\n")
        new_lines.append("#include \"insertion_sort.h\"\n")
        new_lines.append("#include \"merge_sort.h\"\n")
        new_lines.append("#include \"quick_sort.h\"\n")
        new_lines.append("#include \"heap_sort.h\"\n")
        new_lines.append("#include \"radix_sort.h\"\n\n")
        new_lines.append("using namespace std; // correct accordingly\n\n")
        new_lines.append("void saveSortedArray(const std::vector<int>& data, const std::string& algoName, const std::string& inputType, int n, int trialNum) {\n")
        new_lines.append("    mkdir(\"outputs\", 0777);\n")
        new_lines.append("    std::string filename = \"outputs/sorted_\" + algoName + \"_\" + inputType + \"_\" + std::to_string(n) + \"_trial\" + std::to_string(trialNum) + \".txt\";\n")
        new_lines.append("    std::ofstream fout(filename);\n")
        new_lines.append("    if(fout.is_open()) {\n")
        new_lines.append("        for(int val : data) {\n")
        new_lines.append("            fout << val << \"\\n\";\n")
        new_lines.append("        }\n")
        new_lines.append("        fout.close();\n")
        new_lines.append("    }\n")
        new_lines.append("}\n")
        continue

    # 90-91
    if i == 90: # static const int SIZES[]
        new_lines.append("static const int SIZES[] = {100, 500, 1000, 10000, 20000, 50000, 100000};\n")
        continue
    if i == 91: # static const int NUM_SIZES
        new_lines.append("static const int NUM_SIZES = 7;\n")
        continue

    # 110-117 (GLOBAL PIVOT RNG)
    if 110 <= i <= 117:
        continue

    # 183-457 (SORTING ALGORITHMS)
    if 183 <= i <= 457:
        continue
        
    # 525-526: saveSortedArray call
    if "return result;" in line and 510 < i < 530:
        new_lines.append("    saveSortedArray(data, result.algorithm, inputType, result.n, trialNum);\n\n")
        new_lines.append(line)
        continue

    new_lines.append(line)

with open('sort_compare.cpp', 'w') as f:
    f.writelines(new_lines)
