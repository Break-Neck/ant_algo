#ifndef CHOOSER_INCLUDED
#define CHOOSER_INCLUDED

#include <random>
#include <assert.h>

class chooser {
private:
	struct item;
	typedef item* pitem;
	inline static double sum_of(pitem p) { return p ? p->sum : 0; }
	inline static void recalc(pitem p) {
		if (p) {
			p->sum = p->val + sum_of(p->L) + sum_of(p->R);
		}
	}
	struct item {
		pitem L, R;
		double val, prior, sum;
		//Extra data
		int index;
		item(double val, double prior, int index, pitem L = nullptr, pitem R = nullptr) :
			L(L), R(R), val(val), prior(prior), index(index) { recalc(this); }
	};
	pitem root = nullptr;
	static pitem merge(pitem l, pitem r);
	static pitem add(pitem t, pitem new_item) { return merge(t, new_item); }
	static int delete_element(pitem &t, const double cutting); //cutting =/= val !
	static const int BIT_RND = 16;
	static void clear_sub_tree(pitem &t);
public:
	bool empty() const { return !root; }
	template < typename It_type >
	chooser(It_type begin, It_type end) {
		std::random_device rd;
		std::mt19937 gen(rd());
		for (int i = 0; begin != end; ++begin, ++i) {
			if (*begin > 0)
				root = add(root, new item(*begin, std::generate_canonical<double, BIT_RND>(gen),
					i));
		}
	}
	~chooser() {
		clear_sub_tree(root);
		root = nullptr;
	}
	int get_next_index() {
		assert(root);
		std::random_device rd;
		std::mt19937 gen(rd());
		const double cutting = std::generate_canonical<double,BIT_RND>(gen);
		return delete_element(root, cutting * sum_of(root));
	}
};

#endif
