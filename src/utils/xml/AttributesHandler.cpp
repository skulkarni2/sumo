/***************************************************************************
                          AttributesHandler.cpp
                          This class realises the access to the
                            SAX2-Attributes
                             -------------------
    begin                : Mon, 15 Apr 2002
    copyright            : (C) 2002 by Daniel Krajzewicz
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
    Attention!!!
    As one of few, this module is under the
        Lesser GNU General Public Licence
    *********************************************************************
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
 ***************************************************************************/
namespace
{
     const char rcsid[] = "$Id$";
}
// $Log$
// Revision 1.3  2003/09/05 15:28:46  dkrajzew
// secure retrival of boolean values added
//
// Revision 1.2  2003/02/07 10:53:52  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:54:04  dkrajzew
// initial commit for xml-related utility functions
//
// Revision 1.7  2002/06/21 10:50:23  dkrajzew
// inclusion of .cpp-files in .cpp files removed
//
// Revision 1.6  2002/06/11 14:38:22  dkrajzew
// windows eol removed
//
// Revision 1.5  2002/06/11 13:43:35  dkrajzew
// Windows eol removed
//
// Revision 1.4  2002/06/10 08:33:22  dkrajzew
// Parsing of strings into other data formats generelized; Options now recognize false numeric values; documentation added
//
// Revision 1.3  2002/04/17 11:19:56  dkrajzew
// windows-carriage returns removed
//
// Revision 1.2  2002/04/16 06:52:01  dkrajzew
// documentation added; coding standard attachements added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <sax2/Attributes.hpp>
#include <string>
#include <map>
#include <utils/convert/TplConvert.h>
#include <utils/convert/TplConvertSec.h>
#include "AttributesHandler.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
AttributesHandler::AttributesHandler() {
}

AttributesHandler::AttributesHandler(Attr *attrs, int noAttrs)
{
    for(int i=0; i<noAttrs; i++) {
        add(attrs[i].key, attrs[i].name);
    }
}

AttributesHandler::~AttributesHandler() {
    for(AttrMap::iterator i=_tags.begin(); i!=_tags.end(); i++) {
        delete (*i).second;
    }
    _tags.clear();
}

void
AttributesHandler::add(int id, const std::string &name)
{
   check(id);
   _tags.insert(AttrMap::value_type(id, convert(name)));
}

int
AttributesHandler::getInt(const Attributes &attrs, int id) const
{
   return TplConvert<XMLCh>::_2int(
       getAttributeValueSecure(attrs, id));
}

int
AttributesHandler::getIntSecure(const Attributes &attrs, int id,
                                int def) const
{
   return TplConvertSec<XMLCh>::_2intSec(
              getAttributeValueSecure(attrs, id), def);
}

bool
AttributesHandler::getBool(const Attributes &attrs, int id) const
{
   return TplConvert<XMLCh>::_2bool(
              getAttributeValueSecure(attrs, id));
}

bool
AttributesHandler::getBoolSecure(const Attributes &attrs, int id, bool val) const
{
   return TplConvertSec<XMLCh>::_2boolSec(
              getAttributeValueSecure(attrs, id), val);
}

std::string
AttributesHandler::getString(const Attributes &attrs, int id) const
{
   return TplConvert<XMLCh>::_2str(
              getAttributeValueSecure(attrs, id));
}

std::string
AttributesHandler::getStringSecure(const Attributes &attrs, int id,
                                   const std::string &str) const
{
   return TplConvertSec<XMLCh>::_2strSec(
              getAttributeValueSecure(attrs, id), str);
}

long
AttributesHandler::getLong(const Attributes &attrs, int id) const
{
   return TplConvert<XMLCh>::_2long(
              getAttributeValueSecure(attrs, id));
}


long
AttributesHandler::getLongSecure(const Attributes &attrs, int id,
                                  long def) const
{
   return TplConvertSec<XMLCh>::_2longSec(
              getAttributeValueSecure(attrs, id), def);
}


float
AttributesHandler::getFloat(const Attributes &attrs, int id) const
{
   return TplConvert<XMLCh>::_2float(
              getAttributeValueSecure(attrs, id));
}


float
AttributesHandler::getFloatSecure(const Attributes &attrs, int id,
                                  float def) const
{
   return TplConvertSec<XMLCh>::_2floatSec(
              getAttributeValueSecure(attrs, id), def);
}



const XMLCh *const
AttributesHandler::getAttributeNameSecure(int id) const
{
   AttrMap::const_iterator i=_tags.find(id);
   if(i==_tags.end()) {
       throw EmptyData();
   }
   return (*i).second;
}

const XMLCh *
AttributesHandler::getAttributeValueSecure(const Attributes &attrs,
                                           int id) const
{
    return attrs.getValue(getAttributeNameSecure(id));
}

char *
AttributesHandler::getCharP(const Attributes &attrs, int id) const
{
   AttrMap::const_iterator i=_tags.find(id);
   return TplConvert<XMLCh>::_2charp(attrs.getValue(0,
       (*i).second));
}

void
AttributesHandler::check(int id) const
{
    if(_tags.find(id)!=_tags.end()) {
        throw exception();
    }
}

unsigned short *
AttributesHandler::convert(const std::string &name) const
{
   size_t len = name.length();
   unsigned short *ret = new unsigned short[len+1];
   size_t i=0;
   for(; i<len; i++) {
      ret[i] = (unsigned short) name.at(i);
   }
   ret[i] = 0;
   return ret;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "AttributesHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:

