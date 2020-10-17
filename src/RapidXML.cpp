/**
 * RapidXML.cpp
 *
 * Copyright 2019 mikee47 <mike@sillyhouse.net>
 *
 * This file is part of the RapidXML Library
 *
 * This library is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, version 3 or later.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with FlashString.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 ****/

#include "include/RapidXML.h"
#include "../rapidxml/rapidxml_print.hpp"
#include "StringIterator.h"
#include "PrintIterator.h"
#include <setjmp.h>

DEFINE_FSTR(FS_xmlns_xml, "http://www.w3.org/XML/1998/namespace");
DEFINE_FSTR(FS_xmlns_xmlns, "http://www.w3.org/2000/xmlns/");

static jmp_buf xml_exception;

static bool xml_try()
{
	return setjmp(xml_exception) == 0;
}

static void xml_raise()
{
	longjmp(xml_exception, 101);
}

namespace rapidxml
{
void parse_error_handler(const char* what, void* where)
{
	debug_e("RAPIDXML error, %s @ %p", what, where);
	xml_raise();
}
} // namespace rapidxml

namespace XML
{
bool deserialize(Document& doc, char* content)
{
	if(!xml_try()) {
		return false;
	}

	doc.clear();
	doc.parse<0>(content);
	return true;
}

bool deserialize(Document& doc, const FlashString& content)
{
	if(!xml_try()) {
		return false;
	}

	doc.clear();
	auto buffer = doc.allocate_string(content);
	doc.parse<0>(buffer);
	return true;
}

size_t serialize(const Node& node, String& buffer, bool pretty)
{
	auto start_len = buffer.length();
	rapidxml::print(StringIterator(buffer), node, pretty ? 0 : rapidxml::print_no_indenting);
	return buffer.length() - start_len;
}

String serialize(const Node& node, bool pretty)
{
	String buffer;
	serialize(node, buffer, pretty);
	return buffer;
}

size_t serialize(const Node& node, Print& out, bool pretty)
{
	PrintBuffer buffer(out);
	PrintIterator iter(buffer);
	rapidxml::print(iter, node, pretty ? 0 : rapidxml::print_no_indenting);
	return buffer.count();
}

Node* insertDeclaration(Document& doc)
{
	auto first = doc.first_node();
	if(first != nullptr && first->type() == NodeType::node_declaration) {
		return first;
	}

	auto decl = doc.allocate_node(NodeType::node_declaration);
	doc.insert_node(first, decl);
	XML::appendAttribute(decl, _F("version"), "1.0", 7, 3);
	return decl;
}

static Document* getDocument(Node* node)
{
	if(node == nullptr) {
		debug_e("Node is NULL");
		return nullptr;
	}
	auto doc = node->document();
	if(doc == nullptr) {
		debug_e("Node is not in document tree");
	}
	return doc;
}

#define GET_DOCUMENT(node)                                                                                             \
	auto doc = getDocument(node);                                                                                      \
	if(doc == nullptr) {                                                                                               \
		return nullptr;                                                                                                \
	}

Node* appendNode(Node* parent, const char* name, const char* value, size_t name_size, size_t value_size)
{
	GET_DOCUMENT(parent);
	auto p_name = doc->allocate_string(name, (name_size ?: strlen(name)) + 1);
	auto p_value = value ? doc->allocate_string(value, (value_size ?: strlen(value)) + 1) : nullptr;
	auto node = doc->allocate_node(NodeType::node_element, p_name, p_value);
	parent->append_node(node);
	return node;
}

Node* appendNode(Node* parent, const String& name, const FlashString& value)
{
	GET_DOCUMENT(parent);
	auto p_name = doc->allocate_string(name.c_str(), name.length() + 1);
	auto p_value = doc->allocate_string(value);
	auto node = doc->allocate_node(NodeType::node_element, p_name, p_value, name.length(), value.length());
	parent->append_node(node);
	return node;
}

Attribute* appendAttribute(Node* node, const char* name, const char* value, size_t name_size, size_t value_size)
{
	GET_DOCUMENT(node);
	auto p_name = doc->allocate_string(name, (name_size ?: strlen(name)) + 1);
	auto p_value = value ? doc->allocate_string(value, (value_size ?: strlen(value)) + 1) : nullptr;
	auto attr = doc->allocate_attribute(p_name, p_value);
	node->append_attribute(attr);
	return attr;
}

} // namespace XML
