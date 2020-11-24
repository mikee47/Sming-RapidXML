/**
 * RapidXML.h - Helper code for using Rapid XML, with some similarity to ArduinoJson API
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
 * You should have received a copy of the GNU General Public License along with this library.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 ****/

#pragma once

#include <WString.h>
#include <Print.h>
#include <debug_progmem.h>

#define RAPIDXML_NO_EXCEPTIONS
#define RAPIDXML_NO_STREAMS

DECLARE_FSTR(FS_xmlns_xml);
DECLARE_FSTR(FS_xmlns_xmlns);

#include "../../rapidxml/rapidxml.hpp"

namespace XML
{
using Document = rapidxml::xml_document<char>;
using Node = rapidxml::xml_node<char>;
using NodeType = rapidxml::node_type;
using Attribute = rapidxml::xml_attribute<char>;

/**
 * @brief De-serialise XML text into a document
 * @retval bool false on parse error
 * @note Document maintains references to content so MUST stay in scope for lifetime of doc,
 *       or until doc.clear() is called
 * @{
 */
bool deserialize(Document& doc, char* content);

static inline bool deserialize(Document& doc, String& content)
{
	return deserialize(doc, content.begin());
}

bool deserialize(Document& doc, const FlashString& content);
/** @} */

/**
 * @brief Serialize XML text and append to string buffer
 * @retval size_t number of characters written
 * @{
 */
size_t serialize(const Node& node, String& buffer, bool pretty = false);

String serialize(const Node& node, bool pretty = false);

size_t serialize(const Node& node, Print& out, bool pretty = false);

static inline size_t serialize(const Node& node, Print* out, bool pretty = false)
{
	return (out == nullptr) ? 0 : serialize(node, *out, pretty);
}
/** @} */

/**
 * @brief Add a declaration to the document if there isn't one already
 * @note By default, declarations are included when parsed during de-serialisation.
 * If you need one in the output serialisation, call this function.
 * (We're talking about the "<?xml version="1.0" ?>" at the start)
 */
Node* insertDeclaration(Document& doc);

/**
 * @brief Append a child element with optional value
 * @param parent Node to append child to
 * @param name Name of child node (copy taken)
 * @param value Optional node value (copy taken)
 * @retval Node* Child node
 * @note If provide, sizes do not include nul-terminator
 * Node MUST already be added to the document tree
 * @{
 */
Node* appendNode(Node* parent, const char* name, const char* value = nullptr, size_t name_size = 0,
				 size_t value_size = 0);

static inline Node* appendNode(Node* parent, const String& name, const String& value = nullptr)
{
	return appendNode(parent, name.c_str(), value ? value.c_str() : nullptr, name.length(), value.length());
}

Node* appendNode(Node* parent, const String& name, const FlashString& value);

template <typename TString, typename TValue> Node* appendNode(Node* parent, const TString& name, const TValue& value)
{
	return appendNode(parent, String(name), String(value));
}
/** @} */

/**
 * @brief Append an attribute
 * @param parent Node to append child to
 * @param name Name of attribute (copy taken)
 * @param value Value string (copy taken)
 * @param Attribute* New attribute
 * @{
 */
Attribute* appendAttribute(Node* node, const char* name, const char* value, size_t name_size = 0,
						   size_t value_size = 0);

static inline Attribute* appendAttribute(Node* node, const String& name, const String& value)
{
	return appendAttribute(node, name.c_str(), value ? value.c_str() : nullptr, name.length(), value.length());
}

template <typename TString, typename TValue>
Attribute* appendAttribute(Node* node, const TString& name, const TValue& value)
{
	return appendAttribute(node, String(name), String(value));
}
/** @} */

/**
 * @name Gets child node by relative path
 * @param doc
 * @param path Node path using slash as separator
 * @param ns Optional namespace
 * @retval Node* nullptr if none is found
 *
 * Leading/trailing separators are not permitted.
 *
 * @{
 */
Node* getNode(Node* node, const char* path, const char* ns, size_t ns_len = 0);

inline Node* getNode(Node* node, const String& path, const String& ns = nullptr)
{
	return getNode(node, path.c_str(), ns ? ns.c_str() : nullptr, ns.length());
}

/** @} */

/**
 * @name Gets node from a document by path
 * @param doc
 * @param path Node path using slash as separator
 * @param ns Optional namespace
 * @retval Node* nullptr if none is found
 *
 *	Leading separator is important: if present, search starts at root node.
 *	If omitted, first element must match the root node name.
 *
 *	Trailing separator is not allowed.
 *
 *	Example 1: /s:Body/u:GetContentResponse
 *		Will search for s:Body node that is a child of the root node.
 *		The node u:GetContentResponse should be child of the s:Body node
 *
 *	Example 2: s:Envelope/s:Body/u:GetContentResponse
 *		Will search for s:Body node that is a child of the root node named s:Envelope.
 *		The node u:GetContentResponse should be child of the s:Body node
 *
 * @{
 */
Node* getNode(const Document& doc, const char* path, const char* ns = nullptr, size_t ns_len = 0);

inline Node* getNode(const Document& doc, const String& path, const String& ns = nullptr)
{
	return getNode(doc, path.c_str(), ns ? ns.c_str() : nullptr, ns.length());
}
/** @} */

/**
 * @name Get node value
 * @param node
 * @param name
 * @param name_size
 * @retval String invalid if node or name not found
 * @{
 */
String getValue(const Node* node, const char* name, size_t name_size);

inline String getValue(const Node* node, const char* name)
{
	return name ? getValue(node, name, strlen(name)) : nullptr;
}

inline String getValue(const Node* node, const String& name)
{
	return getValue(node, name.c_str(), name.length());
}
/** @} */

/**
 * @name Get attribute value
 * @param node
 * @param name
 * @param name_size
 * @retval String invalid if node or name not found
 * @{
 */
String getAttribute(const Node* node, const char* name, size_t name_size);

inline String getAttribute(const Node* node, const char* name)
{
	return name ? getAttribute(node, name, strlen(name)) : nullptr;
}

inline String getAttribute(const Node* node, const String& name)
{
	return getAttribute(node, name.c_str(), name.length());
}
/** @} */

}; // namespace XML
