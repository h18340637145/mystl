#pragma once
 namespace MiniSTL{

     using rb_tree_color_type = bool ;
     const rb_tree_color_type rb_tree_red = false;
     const rb_tree_color_type rb_tree_black = true; //黑true

     struct __rb_tree_node_base
     {
         //alias declaration
         using color_type = rb_tree_color_type;
         using base_ptr = __rb_tree_node_base*;

        //data member
        color_type color;
        base_ptr parent;
        base_ptr left;
        base_ptr right;

        //get min/max ptr
        static base_ptr minimum(base_ptr root){
            while(root->right){
                root = root->right;
            }
            return root;
        }
     };

     template<class T>
     struct __rb_tree_node : public __rb_tree_node_base{
         using link_type = __rb_tree_node<T>* ;
         T value_field;
     };
}