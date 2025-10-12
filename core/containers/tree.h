#pragma once
#include <vector>
#include <optional>

template <typename T>
struct Tree {
	struct Node {
		T value; // every value should be unique.
		Node* _parent; // may be nullptr.
		std::vector<Node*> _children; // may be empty, but never has nullptr.
	}* _root;
	explicit Tree(const T& value) : _root(_new(value)) {} // every tree is non-empty

	bool add(const T& value, const T& parent) {
		auto* _parent = _find(parent);
		if (_find(value) || !_parent) // TODO: can be optimized.
			return false; // TODO: custom error type

		if (!_root)
			_root = _new(value);
		else
			_attach(_new(value), _parent);
		return true;
	}

	bool remove(const T& value) {
		auto* _node = _find(value);
		if (!_node)
			return false;
		_detach(_node);
		_delete(_node);
		return true;
	}
	bool move(const T& value, const T& parent) {
		auto* _node = _find(value);
		auto* _parent = _find(parent);
		if (!_node || !_parent || _find(_parent->value, _node))
			return false;
		_detach(_node);
		_attach(_node, _parent);
		return true;
	}
	void reorder() {} // TODO

private:
	// every node in arguments should not be nullptr.
	Node* _find(const T& value) {
		return _find(value, _root);
	}

	Node* _find(const T& value, Node* _node) {
		if (_node->value == value)
			return _node;
		for (auto* _child : _node->_children)
			if (auto* _found = _find(value, _child))
				return _found;
		return nullptr;
	}

	Node* _new(const T& value) {
		return new Node{value, nullptr, {}};
	}

	void _attach(Node* _node, Node* _parent) {
		_node->_parent = _parent;
		_parent->_children.push_back(_node);
	}

	void _detach(Node* _node) {
		auto* _parent = _node->_parent;
		if (_parent == nullptr) // root
			return;
		auto& _siblings = _parent->_children;
		_siblings.erase(std::remove(_siblings.begin(), _siblings.end(), _node), _siblings.end());
		_node->_parent = nullptr;
	}

	void _delete(Node* _node) {
		for (auto* _child : _node->_children)
			_delete(_child);
		_node->_children.clear();
		if (_node == _root)
			_root = nullptr;
		delete _node;
	}
};

template <typename T>
struct Forest {
	std::vector<Tree<T>*> _trees;

	bool add(const T& value, std::optional<std::reference_wrapper<const T>> parent = std::nullopt) {
		if (!parent.has_value()) {
			_trees.emplace_back(new Tree<T>(value));
			return true;
		}
		for (auto* _tree : _trees)
			if (_tree->add(value, parent->get()))
				return true;
		return false;
	}
};