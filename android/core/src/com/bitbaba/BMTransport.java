package com.bitbaba;

import java.io.InputStream;
import java.io.OutputStream;

import javax.mail.MessagingException;
import javax.mail.Session;
import javax.mail.URLName;

import org.bouncycastle.crypto.tls.TlsClientProtocol;

import com.sun.mail.smtp.SMTPSSLTransport;

public class BMTransport extends SMTPSSLTransport {

	private MockTlsClient tlsClient = null;
	private TlsClientProtocol protocol = null;
	private InputStream istrm = null;
	private OutputStream ostrm = null;
	
    /**
     * Constructor.
     *
     * @param	session	the Session
     * @param	urlname	the URLName of this transport
     */
    public BMTransport(Session session, URLName urlname) {
		super(session, urlname);
    }
    
    protected int readServerResponse() throws MessagingException {
    	return 0;
    }
    
    protected void sendCommand(String cmd) throws MessagingException {
    	return ;
    }

}
