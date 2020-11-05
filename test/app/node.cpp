/**
 * array.cpp
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
 * You should have received a copy of the GNU General Public License along with RapidXML.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 * @author: Nov 2019 - mikee47 <mike@sillyhouse.net>
 *
 ****/

#include <SmingTest.h>
#include <RapidXML.h>

namespace
{
IMPORT_FSTR(request_xml, PROJECT_DIR "/resource/request.xml")
IMPORT_FSTR(response_xml, PROJECT_DIR "/resource/request.xml")
IMPORT_FSTR(dms_ddd_xml, PROJECT_DIR "/resource/dms-ddd.xml")

DEFINE_FSTR(ns_soap, "http://schemas.xmlsoap.org/soap/envelope/")
DEFINE_FSTR(ns_device, "urn:schemas-upnp-org:device-1-0")
DEFINE_FSTR(ns_dlna, "urn:schemas-dlna-org:device-1-0")

class NodeTest : public TestGroup
{
public:
	NodeTest() : TestGroup(_F("Node"))
	{
	}

#define CHECK(node, str_name)                                                                                          \
	REQUIRE(node != nullptr);                                                                                          \
	REQUIRE(F(str_name) == node->name());

	void execute() override
	{
		TEST_CASE("namespaces")
		{
			XML::Document doc;
			REQUIRE(XML::deserialize(doc, request_xml));
			auto node = doc.first_node();
			CHECK(node, "Envelope");
			node = XML::getNode(doc, "/", ns_soap);
			CHECK(node, "Envelope");
			node = XML::getNode(doc, _F("Envelope"), ns_soap);
			CHECK(node, "Envelope");
			node = XML::getNode(node, _F("Body"), ns_soap);
			CHECK(node, "Body");
			node = XML::getNode(doc, _F("/Body"), ns_soap);
			CHECK(node, "Body");
			node = XML::getNode(doc, _F("Envelope/Body"), ns_soap);
			CHECK(node, "Body");
			node = XML::getNode(doc, _F("Envelope"));
			REQUIRE(node == nullptr);
		}

		TEST_CASE("long paths")
		{
			XML::Document doc;
			REQUIRE(XML::deserialize(doc, dms_ddd_xml));
			auto node = XML::getNode(doc, _F("root/device/iconList/icon/mimetype"), ns_device);
			CHECK(node, "mimetype");
			REQUIRE(F("image/png") == node->value());
			node = XML::getNode(doc, _F("root/device"), ns_device);
			CHECK(node, "device")
			node = XML::getNode(node, _F("X_DLNADOC"), ns_dlna);
			REQUIRE(F("DMS-1.50") == node->value());
		}
	}
};

} // namespace

void REGISTER_TEST(node)
{
	registerGroup<NodeTest>();
}
