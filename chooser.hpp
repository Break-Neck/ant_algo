#ifndef CHOOSER_INCLUDED
#define CHOOSER_INCLUDED

#include <random>
#include <assert.h>

class chooser {
private:
	struct item;
	typedef item* pitem;
	static double sum_of(pitem p) { return p ? p->sum : 0; }
	static void make_parent(pitem parent, pitem child) {
		if (child) {
			child->parent = parent;
		}
		recalc(parent, false);
	}

	static void recalc(pitem p, bool update_parents = true)  {
		if (!p) {
			return;
		}
		p->sum = p->val + sum_of(p->L) + sum_of(p->R);
		if (update_parents) {
			make_parent(p, p->L);
			make_parent(p, p->R);
		}
	}

	struct item {
		int index;
		double val, sum;
		pitem parent, L, R;
		item(double val, int index) : index(index), val(val), sum(val), parent(nullptr),
			L(nullptr), R(nullptr) { recalc(this); }
	};
	pitem root = nullptr;
	inline static void left_rotale(pitem p);
	inline static void right_rotale(pitem p);
	inline static bool is_left_child(pitem p) { assert(p->parent); return p->parent->L == p; }
	static pitem splay(pitem p);
	static pitem sum_lower_bound(pitem tree, double sum);
	inline static pitem leftmost_child(pitem p);
	static pitem delete_element(pitem p);
	static pitem add_element(pitem tree, double val, int index);
	static void clear_sub_tree(pitem p);
	static const int BIT_RND = 16;
public:
	bool empty() const { return !root; }
	template < typename It_type >
	chooser(It_type begin, It_type end) {
		for (int i = 0; begin != end; ++begin, ++i) {
			if (*begin > 0) {
				root = add_element(root, *begin, i);
			}
		}
	}
	~chooser() {
		if (root) {
			clear_sub_tree(root);
		}
		delete root;
	}
	int get_next_index() {
		assert(root);
		//TODO: Test to move that into a new class field
		std::random_device rd;
		std::mt19937 gen(rd());
		const double cutting = std::generate_canonical<double,BIT_RND>(gen);
		pitem res = root = sum_lower_bound(root, cutting * sum_of(root));
		const int ans = res->index;
		root = delete_element(res);
		return ans;
	}
};

#endif