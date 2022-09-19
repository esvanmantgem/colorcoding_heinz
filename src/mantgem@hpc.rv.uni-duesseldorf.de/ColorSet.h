//
// Created by ishuena on 09.10.17.
//

#ifndef HEINZ_COLORSET_H
#define HEINZ_COLORSET_H

namespace colcod {
    class ColorSet {

    public:
        /*
        ColorSet();

        ColorSet(int number_of_colors);

        colcod::ColorSet copy();

        void add(int color);

        void remove(int color);

        void remove(ColorSet set);

        int size();

        std::vector<colcod::ColorSet> create_subsets(int size);

        std::vector<colcod::ColorSet> create_powerset();

        const bool has_color(int color) const;

        bool operator==(ColorSet const &other) const;

        const uint32_t get_colors() const;

        const std::string string() const;*/

    private:
        uint32_t colors;
        int number_of_colors;

        //int get_next(int start);

        //void get_subsets(int size, int start, ColorSet current, std::vector<colcod::ColorSet> *result);


    public:
        ColorSet() {
            colors = 0;
            number_of_colors = 0;
        }

        ColorSet(int nr_of_colors) {
            number_of_colors = 0;
            colors = 0;
            for (int i = 1; i <= nr_of_colors; i++) {
                add(i);
            }
        }

        void add(int color) {
            colors = (colors | (1 << (color - 1)));
            number_of_colors += 1;
        }

        void remove(int color) {
            colors = (colors & ~(1 << (color - 1)));
            number_of_colors -= 1;
        }

        void remove(ColorSet set) {
            colors = colors & (~set.colors);

            int count = 0;

            for (int i = 1; i <= 32; i++) {
                if (has_color(i) && !set.has_color(i)) {
                    count += 1;
                }
            }
            number_of_colors = count;
        }

        int size() {
            return number_of_colors;
        }

        const bool has_color(int color) const {
            return 1 << (color - 1) == (colors & (1 << (color - 1)));
        }

        bool operator==(ColorSet const& other) const {
            return colors == other.colors;
        }

        int get_next(int start) {
            while (!has_color(start)) {
                start++;
            }
            return start;
        }

        ColorSet copy() {
            ColorSet result;
            result.colors = colors;
            result.number_of_colors = number_of_colors;
            return result;
        }

        void get_subsets(int size, int start, ColorSet current, std::vector<colcod::ColorSet> *result) {
            if (current.number_of_colors == size) {
                result->push_back(current.copy());
                return;
            }
            if (start >= 32) return;

            current.add(start);
            get_subsets(size, get_next(start + 1), current, result);
            current.remove(start);
            get_subsets(size, get_next(start + 1), current, result);
        }

        std::vector<colcod::ColorSet> create_subsets(int size) {
            std::vector<colcod::ColorSet> result;
            //std::vector<colcod::ColorSet> temp;
            colcod::ColorSet current;
            //std::cout << "nr of colors: " << number_of_colors << std::endl;
            //for (int i = 1; i <= number_of_colors; i++) {
                get_subsets(size, get_next(0), current, &result);
            //}

            //std::cout << "Powerset created: " << temp.size() << std::endl;
            //for (auto it : temp) {
            //    std::cout << "temp it: " << it.string() << std::endl;
            //    if (it.size() == size) {
            //        result.push_back(it.copy());
            //    }
            //}
            //colcod::ColorSet current;

            //get_subsets(size, get_next(0), current, &result);
            //std::cout << "Nr of subsets: " << result.size() << std::endl;
            return result;
        }

        std::vector<colcod::ColorSet> create_powerset() {
            std::vector<colcod::ColorSet> result;
            colcod::ColorSet current;

            for (int i = 1; i <= number_of_colors; i++) {
                get_subsets(i, get_next(0), current, &result);
            }
            //std::cout << "Nr of subsets in powerset: " << result.size() << std::endl;
            //for (auto it : result) {
            //    std::cout << it.string() << " ";
            //}
            return result;
        }

        //std::string string() {
        //    std::string result = "";
        //    for (int i = 1; i <= number_of_colors; i++) {
        //        result += get_next(i);
        //        result += " ";
        //    }
        //    return result;
        //}


// friendly?
        const uint32_t get_colors() const {
            return colors;
        }

        const std::string string() const {
            std::string result = "";
            for (int i = 1; i <= 32; i++) {
                if (has_color(i)) {
                    result += std::to_string(i) + " ";
                }
            }
            return result;
        }
    };
}

#endif //HEINZ_COLORSET_H
