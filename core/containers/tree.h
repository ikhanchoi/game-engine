#pragma once
#include <vector>
#include <optional>

template <typename T>
struct Tree {
	struct Node {
		T value; // every value should be unique.
		Node* parent; // may be nullptr.
		std::vector<Node*> children; // may be empty, but never has nullptr.
	}* root;
	explicit Tree(const T& value) : root(_new(value)) {} // every tree is non-empty

	bool add(const T& value, const T& parentValue) {
		auto* parent = _find(parentValue);
		if (_find(value) || !parent) // TODO: can be optimized.
			return false; // TODO: custom error type

		if (!root)
			root = _new(value);
		else
			_attach(_new(value), parent);
		return true;
	}

	bool remove(const T& value) {
		auto* node = _find(value);
		if (!node)
			return false;
		_detach(node);
		_delete(node);
		return true;
	}

	bool move(const T& value, const T& parentValue) {
		auto* node = _find(value);
		auto* parent = _find(parentValue);
		if (!node || !parent || _find(parent->value, node))
			return false;
		_detach(node);
		_attach(node, parent);
		return true;
	}

	template <typename Function>
	void traverse(Function&& function) {
		_traverse(root, std::forward<Function>(function));
	}

	void reorder() {} // TODO

private:
	// every node in arguments should not be nullptr.
	Node* _find(const T& value) {
		return _find(value, root);
	}

	Node* _find(const T& value, Node* node) {
		if (node->value == value)
			return node;
		for (auto* child : node->children)
			if (auto* found = _find(value, child))
				return found;
		return nullptr;
	}

	Node* _new(const T& value) {
		return new Node{value, nullptr, {}};
	}

	void _attach(Node* node, Node* parent) {
		node->parent = parent;
		parent->children.push_back(node);
	}

	void _detach(Node* node) {
		auto* parent = node->parent;
		if (parent == nullptr) // root
			return;
		auto& siblings = parent->children;
		siblings.erase(std::remove(siblings.begin(), siblings.end(), node), siblings.end());
		node->parent = nullptr;
	}

	void _delete(Node* node) {
		for (auto* child : node->children)
			_delete(child);
		node->children.clear();
		if (node == root)
			root = nullptr;
		delete node;
	}

	template <typename Function>
	void _traverse(Node* node, Function&& function) {
		function(node->value);
		for (auto* child : node->children)
			_traverse(child, std::forward<Function>(function));
	}
};

template <typename T>
struct Forest {
	std::vector<Tree<T>*> trees;

	bool add(const T& value, std::optional<std::reference_wrapper<const T>> parent = std::nullopt) {
		if (!parent.has_value()) {
			trees.emplace_back(new Tree<T>(value));
			return true;
		}
		for (auto* tree : trees)
			if (tree->add(value, parent->get()))
				return true;
		return false;
	}

	bool remove(const T& value) {
		for (auto* tree : trees)
			if (tree->remove(value)) {
				if (tree->root == nullptr) {
					trees.erase(std::remove(trees.begin(), trees.end(), tree), trees.end());
					delete tree;
				}
				return true;
			}
		return false;
	}

	template <typename Function>
	void traverse(Function&& function) {
		for (auto* tree : trees)
			tree->traverse(std::forward<Function>(function));
	}
};