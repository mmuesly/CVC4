; COMMAND-LINE: --strings-exp
(set-option :incremental true)
(set-logic SLIA)
(declare-const x String)

(push)
(assert (= "AFOOE" (str.replace_re x (re.++ (str.to_re "B") re.allchar (str.to_re "C")) "FOO")))
(assert (not (= x "AFOOE")))
(set-info :status sat)
(check-sat)
(pop)

(push)
(assert (= "FOOA" (str.replace_re x re.all "FOO")))
(set-info :status sat)
(check-sat)
(pop)

(push)
(assert (= "FOOA" (str.replace_re x (re.++ (str.to_re "A") re.all) "FOO")))
(set-info :status sat)
(check-sat)
(pop)

(push)
(assert (= "FOOA" (str.replace_re x (re.++ (str.to_re "A") re.all) "FOO")))
(assert (> (str.len x) 2))
(set-info :status unsat)
(check-sat)
(pop)

(push)
(assert (= "FOOA" (str.replace_re "A" re.all "FOO")))
(set-info :status sat)
(check-sat)
(pop)

(push)
(assert (not (= "A" (str.replace_re "A" re.none "FOO"))))
(set-info :status unsat)
(check-sat)
(pop)