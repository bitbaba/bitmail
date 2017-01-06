/*
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
 *
 * Copyright (c) 1997-2016 Oracle and/or its affiliates. All rights reserved.
 *
 * The contents of this file are subject to the terms of either the GNU
 * General Public License Version 2 only ("GPL") or the Common Development
 * and Distribution License("CDDL") (collectively, the "License").  You
 * may not use this file except in compliance with the License.  You can
 * obtain a copy of the License at
 * https://glassfish.dev.java.net/public/CDDL+GPL_1_1.html
 * or packager/legal/LICENSE.txt.  See the License for the specific
 * language governing permissions and limitations under the License.
 *
 * When distributing the software, include this License Header Notice in each
 * file and include the License file at packager/legal/LICENSE.txt.
 *
 * GPL Classpath Exception:
 * Oracle designates this particular file as subject to the "Classpath"
 * exception as provided by Oracle in the GPL Version 2 section of the License
 * file that accompanied this code.
 *
 * Modifications:
 * If applicable, add the following below the License Header, with the fields
 * enclosed by brackets [] replaced by your own identifying information:
 * "Portions Copyright [year] [name of copyright owner]"
 *
 * Contributor(s):
 * If you wish your version of this file to be governed by only the CDDL or
 * only the GPL Version 2, indicate your decision by adding "[Contributor]
 * elects to include this software in this distribution under the [CDDL or GPL
 * Version 2] license."  If you don't indicate a single choice of license, a
 * recipient has the option to distribute your version of this file under
 * either the CDDL, the GPL Version 2 or to extend the choice of license to
 * its licensees as provided above.  However, if you add GPL Version 2 code
 * and therefore, elected the GPL Version 2 license, then the option applies
 * only if the new code is made subject to such option by the copyright
 * holder.
 */

package com.bitbaba;

import java.io.*;
import java.net.*;
import java.util.*;
import java.util.logging.Level;
import java.lang.reflect.*;
import javax.net.ssl.SSLSocket;

import javax.mail.*;
import javax.mail.event.*;
import javax.mail.internet.*;

import com.sun.mail.util.*;
import com.sun.mail.auth.*;
import com.sun.mail.smtp.DigestMD5;
import com.sun.mail.smtp.SMTPAddressFailedException;
import com.sun.mail.smtp.SMTPAddressSucceededException;
import com.sun.mail.smtp.SMTPMessage;
import com.sun.mail.smtp.SMTPOutputStream;
import com.sun.mail.smtp.SMTPSendFailedException;
import com.sun.mail.smtp.SMTPSenderFailedException;
import com.sun.mail.smtp.SMTPTransport;
import com.sun.mail.smtp.SaslAuthenticator;

/**
 * This class implements the Transport abstract class using SMTP for
 * message submission and transport. <p>
 *
 * See the <a href="package-summary.html">com.sun.mail.smtp</a> package
 * documentation for further information on the SMTP protocol provider. <p>
 *
 * This class includes many protected methods that allow a subclass to
 * extend this class and add support for non-standard SMTP commands.
 * The {@link #issueCommand} and {@link #sendCommand} methods can be
 * used to send simple SMTP commands.  Other methods such as the
 * {@link #mailFrom} and {@link #data} methods can be overridden to
 * insert new commands before or after the corresponding SMTP commands.
 * For example, a subclass could do this to send the XACT command
 * before sending the DATA command:
 * <pre>
 *	protected OutputStream data() throws MessagingException {
 *	    if (supportsExtension("XACCOUNTING"))
 *	        issueCommand("XACT", 25);
 *	    return super.data();
 *	}
 * </pre>
 *
 * @author Max Spivak
 * @author Bill Shannon
 * @author Dean Gibson (DIGEST-MD5 authentication)
 * @author Lu\u00EDs Serralheiro (NTLM authentication)
 *
 * @see javax.mail.event.ConnectionEvent
 * @see javax.mail.event.TransportEvent
 */

public class BMTransport extends SMTPTransport {

	protected BMTransport(Session session, URLName urlname, String name, boolean isSSL) {
		super(session, urlname, name, isSSL);
		// TODO Auto-generated constructor stub
	}	
}
