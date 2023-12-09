(ns advent-of-code-2023.day01)
(require 'clojure.string)
(def test-input (clojure.string/split-lines (slurp "inputs/day01_test.txt")))
(defn calibration-value [line]
  (let [digits (filter #(Character/isDigit %) (seq line))
        number-str (apply str [(first digits) (last digits)])]
    (Integer. number-str)))

(defn part-1 [input]
  (reduce + (map calibration-value input)))