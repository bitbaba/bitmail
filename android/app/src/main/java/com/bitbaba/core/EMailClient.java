/**
 * 
 */
package com.bitbaba.core;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Properties;

import javax.activation.DataHandler;
import javax.activation.DataSource;
import javax.mail.FetchProfile;
import javax.mail.Flags;
import javax.mail.Folder;
import javax.mail.Message;
import javax.mail.MessagingException;
import javax.mail.NoSuchProviderException;
import javax.mail.Session;
import javax.mail.Store;
import javax.mail.Transport;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeMessage;
import javax.mail.search.FlagTerm;

/**
 * @author Administrator
 *
 */
public class EMailClient {

	private static int POP3_SSL_DEFAULT_PORT = 995;
	private static int SMTP_SSL_DEFAULT_PORT = 465; // or 994
	private static int IMAP_SSL_DEFAULT_PORT = 993;
	
	
	private String smtp_ = null;
	private String imap_ = null;
	
	private String login_ = null;
	private String password_ = null;
	
	private final static String BITMAIL_SUBJECT = "BitMail";
	
	/**
	 * 
	 * @param login, usually the email address
	 * @param password, secret to login into smtp/imap server
	 */
	public EMailClient(String login, String password){
		login_ = login;
		if (login_.indexOf("@") != -1){
			smtp_ = "smtp." + GetHostFromEmailAddress(login_);
			imap_ = "imap." + GetHostFromEmailAddress(login_);
		}
		password_ = password;
	}
		
	public boolean Send(String to, String content) 
	{
		Properties properties = new Properties();
		
		properties.put("mail.smtps.ssl.enable", "true");
		properties.put("mail.smtps.host", smtp_);
		properties.put("mail.smtps.auth", "true");
		//properties.put("mail.smtps.ssl.socketFactory.class", "com.bitbaba.TSLSocketConnectionFactory");
		properties.put("mail.smtps.ssl.socketFactory.class", "javax.net.ssl.SSLSocketFactory");
		properties.put("mail.smtps.socketFactory.fallback", "false");
		properties.put("mail.smtps.ssl.socketFactory.port", SMTP_SSL_DEFAULT_PORT);

		Session session = Session.getInstance(properties);		
			
		Message message = new MimeMessage(session);
		try {
			message.setFrom(new InternetAddress(login_));
			message.setRecipient(MimeMessage.RecipientType.TO, new InternetAddress(to));
			message.setSubject(BITMAIL_SUBJECT);
			message.setSentDate(new Date());
			DataHandler handler = new DataHandler(new ByteArrayDataSource(content.getBytes(), "text/plain"));
			message.setText(content);
			//message.saveChanges();
		} catch (MessagingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		Transport trans = null;
		try {
			trans = session.getTransport("smtps");
		} catch (NoSuchProviderException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}		

		try {				
			trans.connect(smtp_, SMTP_SSL_DEFAULT_PORT, login_, password_);
		} catch (MessagingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}		
		
		try {
			trans.sendMessage(message, message.getAllRecipients());	
		} catch (MessagingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		try {
			trans.close();
		} catch (MessagingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return true;
	}
	public class ByteArrayDataSource implements DataSource {
		private byte[] data;
		private String type;

		public ByteArrayDataSource(byte[] data, String type) {
			super();
			this.data = data;
			this.type = type;
		}

		public ByteArrayDataSource(byte[] data) {
			super();
			this.data = data;
		}

		public void setType(String type) {
			this.type = type;
		}

		public String getContentType() {
			if (type == null)
				return "application/octet-stream";
			else
				return type;
		}

		public InputStream getInputStream() throws IOException {
			return new ByteArrayInputStream(data);
		}

		public String getName() {
			return "ByteArrayDataSource";
		}

		public OutputStream getOutputStream() throws IOException {
			throw new IOException("Not Supported");
		}
	}

	public List<String> Receive() 
	{
		List<String> results = new ArrayList<String>();
		
		Properties properties = new Properties();
		
		properties.put("mail.imaps.ssl.enable", "true");
		properties.put("mail.imaps.host", imap_);
		properties.put("mail.imaps.auth", "true");
		properties.put("mail.imaps.ssl.socketFactory.class", "com.bitbaba.TSLSocketConnectionFactory");		
		properties.put("mail.imaps.socketFactory.fallback", "false");
		properties.put("mail.imaps.ssl.socketFactory.port", IMAP_SSL_DEFAULT_PORT);

		Session session = Session.getInstance(properties);	
		
		Store store = null;
		try {
			store = session.getStore("imaps");
		} catch (NoSuchProviderException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}		

		try {				
			store.connect(imap_, IMAP_SSL_DEFAULT_PORT, login_, password_);			
		} catch (MessagingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return null;
		}
		
		Folder inbox = null;
		try {
			inbox = store.getFolder("INBOX");
		} catch (MessagingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		if (inbox == null){
			try {
				store.close();
			} catch (MessagingException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			return null;
		}
		
		try {
			inbox.open(Folder.READ_WRITE);
		} catch (MessagingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		if (!inbox.isOpen()){
			try {
				store.close();
			} catch (MessagingException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			return null;
		}
		
		Message [] messages = null;
		
		try {
			// HarryWu, server-side search for `unseen' messages. 
			FlagTerm flagTerm = new FlagTerm(new Flags(Flags.Flag.SEEN), false);			
			messages = inbox.search(flagTerm);
		} catch (MessagingException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		if (messages.length == 0){
			try {
				inbox.close(true);
			} catch (MessagingException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			try {
				store.close();
			} catch (MessagingException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return null;
		}
		
		// HarryWu, do pre-fetch, in a batch call.
		FetchProfile fp = new FetchProfile();
		fp.add(FetchProfile.Item.CONTENT_INFO);
		fp.add(FetchProfile.Item.ENVELOPE);
		fp.add(FetchProfile.Item.FLAGS);

		System.out.println("Pre-fetch messages");
		
		try {
			inbox.fetch(messages, fp);
		} catch (MessagingException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
				
		// HarryWu, Analyze a message
		for (int i = 0; i < messages.length; ++i){
			Message elt = messages[i];			
			System.out.println("[" + i + "] Msgno: " + elt.getMessageNumber());
			String subject = null;
			try {
				subject = (elt.getSubject());
			} catch (MessagingException e) {
				// TODO Auto-generated catch block
				continue;
			}
			if (subject == null || subject.compareTo(BITMAIL_SUBJECT) != 0){
				continue;
			}
			try {
				if (elt.isMimeType("text/plain")){
					results.add((String) elt.getContent());
					elt.setFlag(Flags.Flag.DELETED, true);
				}								
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				continue;
			} catch (MessagingException e) {
				// TODO Auto-generated catch block
				continue;
			}		
		}
		
		try {
			inbox.close(true);
		} catch (MessagingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}		
		
		try {
			store.close();
		} catch (MessagingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		return results;
	}
	
	public static String GetHostFromEmailAddress(String email)
	{
		if (email.indexOf("@") == -1){
			return "";
		}
		return email.substring(email.indexOf("@")+1);
	}	
}


