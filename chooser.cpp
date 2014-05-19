#include "chooser.hpp"

chooser::pitem chooser::merge(chooser::pitem l, chooser::pitem r) {
	if (!l || !r) {
		return l ? l : r;
	}
	if (l->prior > r->prior) {
		l->R = merge(l->R, r);
		recalc(l);
		return l;
	} else {
		r->L = merge(l, r->L);
		recalc(r);
		return r;
	}
}

int chooser::delete_element(chooser::pitem &t, const double cutting) {
	assert(t);
	assert(cutting <= sum_of(t));
	int ans = -1;
	if (cutting > sum_of(t->L) + t->val) {
		ans = delete_element(t->R, cutting - (sum_of(t->L) + t->val));
		recalc(t);
	} else if (cutting < sum_of(t->L)) {
		ans = delete_element(t->L, cutting);
		recalc(t);
	} else {
		ans = t->index;
		pitem _t = t;
		t = merge(t->L, t->R);
		delete _t;
	}
	return ans;
}

void chooser::clear_sub_tree(chooser::pitem &t) {
	if (!t) {
		return;
	}
	if (t->L) {
		clear_sub_tree(t->L);
	}
	if (t->R) {
		clear_sub_tree(t->R);
	}
	delete t;
	t = nullptr;
}
