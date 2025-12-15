#include <iostream>
#include <cassert>

template<class TYPE>
struct Node {
public:
    TYPE data;
    Node *left;
    Node *right;
    std::size_t height;
    std::size_t subtreeSize;
public:
    Node(const TYPE &data) 
    : data(data), left(nullptr), right(nullptr), height(1), subtreeSize(1) {}
};

template<class TYPE>
using NodePtr = Node<TYPE> *;

template<
    class TYPE,
    class COMPARATOR = std::less<TYPE>
>
class AVLTree {
private: // private fields
    NodePtr<TYPE> _root;
    COMPARATOR _cmp;

private: // help methods
    enum class CmpResult {
        Less,
        Equal,
        Greater
    };

    CmpResult compare(const TYPE &a, const TYPE &b) noexcept {
        if (_cmp(a, b) == true) {
            return CmpResult::Less;
        }
        if (_cmp(b, a) == true) {
            return CmpResult::Greater;
        }

        return CmpResult::Equal;
    }

    void _destroyTree(NodePtr<TYPE> node) {
        if (node == nullptr) {
            return;
        }
        _destroyTree(node->left);
        _destroyTree(node->right);
        delete node;
    }
    
    std::size_t _getHeight(const NodePtr<TYPE> node) const noexcept {
        return node ? node->height : 0;
    }

    std::size_t _getSubtreeSize(const NodePtr<TYPE> node) const noexcept {
        return node ? node->subtreeSize : 0;
    }

    void _updateNode(NodePtr<TYPE> node) noexcept {
        if (node == nullptr) {
            return;
        }
        
        node->height = std::max(
            _getHeight(node->left),
            _getHeight(node->right)
        ) + 1;

        node->subtreeSize = _getSubtreeSize(node->left) + _getSubtreeSize(node->right) + 1;
    }

    int _getTreeBalance(NodePtr<TYPE> root) const noexcept {
        return _getHeight(root->right) - _getHeight(root->left);
    }

    NodePtr<TYPE> _rotateLeft(NodePtr<TYPE> root) noexcept {
        NodePtr<TYPE> temp = root->right;
        root->right = temp->left;
        temp->left = root;

        _updateNode(root);
        _updateNode(temp);

        return temp;
    }

    NodePtr<TYPE> _rotateRight(NodePtr<TYPE> root) noexcept {
        NodePtr<TYPE> temp = root->left;
        root->left = temp->right;
        temp->right = root;

        _updateNode(root);
        _updateNode(temp);

        return temp;
    }

    NodePtr<TYPE> _balanceTree(NodePtr<TYPE> root) noexcept {
        _updateNode(root);

        int heightDifference = _getTreeBalance(root);
        if (heightDifference == 2) {
            if (_getTreeBalance(root->right) < 0) {
                root->right = _rotateRight(root->right);
            }
            return _rotateLeft(root);
        }
        else if (heightDifference == -2) {
            if (_getTreeBalance(root->left) > 0) {
                root->left = _rotateLeft(root->left);
            }
            return _rotateRight(root);
        }
        
        return root;
    }

    NodePtr<TYPE> _findMin(NodePtr<TYPE> root) {
        while (root->left) {
            root = root->left;
        }

        return root;
    }

    NodePtr<TYPE> _removeMin(NodePtr<TYPE> node) {
        if (node->left == nullptr) {
            return node->right;
        }
        node->left = _removeMin(node->left);
        return _balanceTree(node);
    }

    NodePtr<TYPE> _addInternal(
        NodePtr<TYPE> node,
        const TYPE &data,
        std::size_t &pos
    ) {
        if (node == nullptr) {
            return new Node<TYPE>(data);
        }
        
        // data < node->data
        if (compare(data, node->data) == CmpResult::Less) {
            node->left = _addInternal(node->left, data, pos);
        }
        // node->data <= data
        else {
            pos += _getSubtreeSize(node->left) + 1;
            node->right = _addInternal(node->right, data, pos); 
        }   

        return _balanceTree(node);
    }

    NodePtr<TYPE> _deleteInternal(
        NodePtr<TYPE> node,
        std::size_t pos
    ) noexcept {
        if (node == nullptr) {
            return nullptr;
        }

        std::size_t leftSize = _getSubtreeSize(node->left);

        if (pos < leftSize) {
            node->left = _deleteInternal(node->left, pos);
        }
        else if (pos > leftSize) {
            node->right = _deleteInternal(node->right, pos - leftSize - 1);
        }
        else {
            NodePtr<TYPE> leftTemp = node->left;
            NodePtr<TYPE> rightTemp = node->right;

            delete node;

            if (rightTemp == nullptr) {
                return leftTemp;
            }
            if (leftTemp == nullptr) {
                return rightTemp;
            }

            NodePtr<TYPE> min = _findMin(rightTemp);
            min->right = _removeMin(rightTemp);
            min->left = leftTemp;

            return _balanceTree(min);
        }

        return _balanceTree(node);
    }

public: // stuff methods
    AVLTree()
    : _root(nullptr), _cmp() {}
    
    ~AVLTree() {
        _destroyTree(_root);
    }

public: // public interface
    std::size_t add(const TYPE& data) {
        std::size_t pos = 0;
        _root = _addInternal(_root, data, pos);
        return pos;
    }


    void remove(std::size_t pos) {
        _root = _deleteInternal(_root, pos);
    }

    bool exists(const TYPE &data) {
        NodePtr<TYPE> temp = _root;
        while (temp) {
            auto c = compare(temp->data, data);
            if (c == CmpResult::Equal) {
                return true; // temp->data == data
            }
            else if (c == CmpResult::Less) {
                temp = temp->right; // temp->data < data
            }
            else {
                temp = temp->left; // temp->data > data
            }
        }

        return false;
    }
};

int main() {
    using Tree = AVLTree<std::size_t, std::greater<std::size_t>>;
    using Command = void (*)(
        Tree &,
        std::size_t
    );
    Command commands[2];

    commands[0] = [](
        Tree &tree,
        std::size_t height
    ) {
        std::cout << tree.add(height) << std::endl;
    };

    commands[1] = [](
        Tree &tree,
        std::size_t position
    ) {
        tree.remove(position);
    };

    Tree tree;

    std::size_t n;
    std::cin >> n;
    while (n--) {
        std::size_t command, data;
        std::cin >> command >> data;

        commands[command - 1](tree, data);
    }

    return 0;
}