(set-logic ALL)
(set-info :status sat)
(declare-fun a () Int)
(declare-fun b () Int)
(declare-fun c () Int)
(assert (= (choice ((x Int)) (= x a)) 1))
(assert (= (choice ((x Int)) (= x b)) 2))
;(assert (let ((x (choice ((x Int)) true))) (and (distinct a b x)(= x c))))
(check-sat)
