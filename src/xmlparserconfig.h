/*
 * xmlparserconfig.h
 *
 *  Created on: Jul 18, 2012
 *      Author: cesar
 */

#ifndef XMLPARSERCONFIG_H_
#define XMLPARSERCONFIG_H_

xmlDocPtr extract_subdocument(xmlTextReaderPtr reader, const xmlChar *pattern);
xmlXPathObjectPtr execute_xpath_expression(xmlDocPtr doc, const xmlChar* xpathExpr);
int  register_namespaces(xmlXPathContextPtr xpathCtx, const xmlChar* nsList);
void print_xpath_nodes(xmlNodeSetPtr nodes, FILE* output);
void* xgetOneParameter(xmlDocPtr doc, char *xpath);
xmlChar* xgetPropertyCommunity(xmlDocPtr doc, xmlChar* node, xmlChar* property);


#endif /* XMLPARSERCONFIG_H_ */
