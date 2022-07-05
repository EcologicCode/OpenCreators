/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is JavaScript Engine testing utilities.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communication Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

gTestfile = 'while-003.js';

/**
 *  File Name:          while-003
 *  ECMA Section:
 *  Description:        while statement
 *
 *  The while expression evaluates to true, Statement returns abrupt completion.
 *
 *  Author:             christine@netscape.com
 *  Date:               11 August 1998
 */
var SECTION = "while-003";
var VERSION = "ECMA_2";
var TITLE   = "while statement";

startTest();
writeHeaderToLog( SECTION + " "+ TITLE);

DoWhile( new DoWhileObject(
	   "while expression is true",
	   true,
	   "result = \"pass\";" ));

DoWhile( new DoWhileObject(
	   "while expression is 1",
	   1,
	   "result = \"pass\";" ));

DoWhile( new DoWhileObject(
	   "while expression is new Boolean(false)",
	   new Boolean(false),
	   "result = \"pass\";" ));

DoWhile( new DoWhileObject(
	   "while expression is new Object()",
	   new Object(),
	   "result = \"pass\";" ));

DoWhile( new DoWhileObject(
	   "while expression is \"hi\"",
	   "hi",
	   "result = \"pass\";" ));
/*
  DoWhile( new DoWhileObject(
  "while expression has a continue in it",
  "true",
  "if ( i == void 0 ) i = 0; result=\"pass\"; if ( ++i == 1 ) {continue;} else {break;} result=\"fail\";"
  ));
*/
test();

function DoWhileObject( d, e, s ) {
  this.description = d;
  this.whileExpression = e;
  this.statements = s;
}

function DoWhile( object ) {
  result = "fail:  statements in while block were not evaluated";

  while ( expression = object.whileExpression ) {
    eval( object.statements );
    break;
  }

  // verify that the while expression was evaluated

  new TestCase(
    SECTION,
    "verify that while expression was evaluated (should be "+
    object.whileExpression +")",
    "pass",
    (object.whileExpression == expression ||
     ( isNaN(object.whileExpression) && isNaN(expression) )
      ) ? "pass" : "fail" );

  new TestCase(
    SECTION,
    object.description,
    "pass",
    result );
}
