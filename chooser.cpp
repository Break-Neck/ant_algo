#include "chooser.hpp"

inline void chooser::left_rotale(pitem p) {
	assert(p->R);
	pitem q = p->R;
	if (p->parent) {
		if (is_left_child(p)) {
			p->parent->L = q;
		} else {
			p->parent->R = q;
		}
	}
	p->R = q->L;
	q->L = p;
	recalc(p->R);
	recalc(p);
	make_parent(p->parent, q);
	make_parent(q, p);
	recalc(q);
	recalc(q->parent);
}


inline void chooser::right_rotale(pitem p) {
	assert(p->L);
	pitem q = p->L;
	if (p->parent) {
		if (is_left_child(p)) {
			p->parent->L = q;
		} else {
			p->parent->R = q;
		}
	}
	p->L = q->R;
	q->R = p;
	recalc(p->L);
	recalc(p);
	make_parent(p->parent, q);
	make_parent(q, p);
	recalc(q);
	recalc(q->parent);
}

chooser::pitem chooser::splay(pitem p) {
	if (!p) {
		return nullptr;
	}
	while (p->parent) {
		if (!p->parent->parent) {
			if (is_left_child(p)) {
				right_rotale(p->parent);
			} else {
				left_rotale(p->parent);
			}
		} else if (is_left_child(p) ^ is_left_child(p->parent)) {
			if (is_left_child(p)) {
				right_rotale(p->parent);
				left_rotale(p->parent);
			} else {
				left_rotale(p->parent);
				right_rotale(p->parent);
			}
		} else {
			if (is_left_child(p)) {
				right_rotale(p->parent->parent);
				right_rotale(p->parent);
			} else {
				left_rotale(p->parent->parent);
				left_rotale(p->parent);
			}
		}
	}
	return p;
}

chooser::pitem chooser::sum_lower_bound(pitem tree, double sum) {
	assert(tree);
	pitem p = tree;
	while (true) {
		if (sum_of(p->L) >= sum) {
			p = p->L;
		} else if (sum_of(p->L) + p->val >= sum) {
			break;
		} else {
			sum -= sum_of(p->L) + p->val;
			p = p->R;
		}
	}
	assert(p);
	return splay(p);
}

chooser::pitem chooser::leftmost_child(pitem p) {
	while (p->L) {
		p = p->L;
	}
	return p;
}


chooser::pitem chooser::delete_element(pitem p) {
	pitem res = nullptr;
	if (!p->L && !p->R) {
		if (p->parent) {
			res = p->parent;
			if (is_left_child(p)) {
				res->L = nullptr;
			} else {
				res->R = nullptr;
			}
		}
		delete p;
	} else if (!p->L || !p->R) {
		if (p->parent) {
			res = p->parent;
			if (is_left_child(p)) {
				res->L = p->L ? p->L : p->R;
			} else {
				res->R = p->L ? p->L : p->R;
			}
			p->L = p->R = nullptr;
			delete p;
		} else {
			if (p->L) {
				right_rotale(p);
			} else {
				left_rotale(p);
			}
			return delete_element(p);
		}
	} else {
		pitem q = leftmost_child(p->R);
		if (q != p->R) {
			std::swap(p->L, q->L);
			std::swap(p->R, q->R);
			recalc(p);
			recalc(q);
			const pitem p_par = p->parent;
			make_parent(q->parent, p);
			make_parent(p_par, q);
		} else {
			left_rotale(p);
		}
		assert(!p->L || !p->R);
		delete_element(p);
		res = q->parent ? q->parent : q;
	}
	recalc(res);
	return splay(res);
}


chooser::pitem chooser::add_element(pitem tree, double val, int index) {
	pitem newpi = new item(val, index);
	if (!tree) {
		return newpi;
	}
	pitem p = tree;
	while ((p->val > val && p->L) || (p->val < val && p->R)) {
		p = p->val > val ? p->L : p->R;
	}
	if (p->val > val) {
		p->L = newpi;	
	} else {
		p->R = newpi;
	}
	recalc(p);
	return splay(newpi);
}

void chooser::clear_sub_tree(pitem p) {
	if (p->L) {
		clear_sub_tree(p->L);
	}
	if (p->R) {
		clear_sub_tree(p->R);
	}
	delete p->L;
	delete p->R;
	p->L = p->R = nullptr;
}