package com.bitbaba.core;

import org.bouncycastle.crypto.tls.TlsClient;
import org.bouncycastle.crypto.tls.TlsClientProtocol;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.security.SecureRandom;

import javax.net.SocketFactory;
import javax.net.ssl.HandshakeCompletedListener;
import javax.net.ssl.SSLSession;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;


// http://stackoverflow.com/questions/33517476/tls-1-2-java-1-6-bouncycastle
public class TSLSocketConnectionFactory extends SSLSocketFactory {
	public TSLSocketConnectionFactory(){
		System.out.println("TSLSocketConnectionFactory");
	}
	
	public static SocketFactory getDefault(){
		return new TSLSocketConnectionFactory();
	}
	

    @Override
    public Socket createSocket(Socket socket, final String host, int port, boolean arg3)
            throws IOException {
        if (socket == null) {
            socket = new Socket();
        }
        if (!socket.isConnected()) {
            socket.connect(new InetSocketAddress(host, port));
        }

        final TlsClientProtocol tlsClientProtocol = new TlsClientProtocol(socket.getInputStream(), socket.getOutputStream(), new SecureRandom());
       
       
        final TlsClient tlsClient = new MockTlsClient(null);
        
        
        tlsClientProtocol.connect(tlsClient);
        
        return new SSLSocket(){
            @Override
            public InputStream getInputStream() throws IOException {
                return tlsClientProtocol.getInputStream();
            }

            @Override
            public OutputStream getOutputStream() throws IOException {
                return tlsClientProtocol.getOutputStream();
            }

            @Override
            public synchronized void close() throws IOException {         
               tlsClientProtocol.close();
            }
            
			@Override
			public void addHandshakeCompletedListener(HandshakeCompletedListener listener) {
				// TODO Auto-generated method stub
			}

			@Override
			public boolean getEnableSessionCreation() {
				// TODO Auto-generated method stub
				return false;
			}

			@Override
			public String[] getEnabledCipherSuites() {
				// TODO Auto-generated method stub
				return null;
			}

			@Override
			public String[] getEnabledProtocols() {
				// TODO Auto-generated method stub
				String [] versions = {
					"TLSv1.2"
				};
				return versions;
			}

			@Override
			public boolean getNeedClientAuth() {
				// TODO Auto-generated method stub
				return false;
			}

			@Override
			public SSLSession getSession() {
				// TODO Auto-generated method stub
				return null;
			}

			@Override
			public String[] getSupportedCipherSuites() {
				// TODO Auto-generated method stub
				return null;
			}

			@Override
			public String[] getSupportedProtocols() {
				// TODO Auto-generated method stub
				return null;
			}

			@Override
			public boolean getUseClientMode() {
				// TODO Auto-generated method stub
				return false;
			}

			@Override
			public boolean getWantClientAuth() {
				// TODO Auto-generated method stub
				return false;
			}

			@Override
			public void removeHandshakeCompletedListener(HandshakeCompletedListener listener) {
				// TODO Auto-generated method stub
				
			}

			@Override
			public void setEnableSessionCreation(boolean flag) {
				// TODO Auto-generated method stub
				
			}

			@Override
			public void setEnabledCipherSuites(String[] suites) {
				// TODO Auto-generated method stub
				
			}

			@Override
			public void setEnabledProtocols(String[] protocols) {
				// TODO Auto-generated method stub
				
			}

			@Override
			public void setNeedClientAuth(boolean need) {
				// TODO Auto-generated method stub
				
			}

			@Override
			public void setUseClientMode(boolean mode) {
				// TODO Auto-generated method stub
				
			}

			@Override
			public void setWantClientAuth(boolean want) {
				// TODO Auto-generated method stub
				
			}

			@Override
			public void startHandshake() throws IOException {
				// TODO Auto-generated method stub
				System.out.println("startHandshake");
			}
        	
        };
      }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SOCKET FACTORY  METHODS  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    @Override
    public String[] getDefaultCipherSuites() {      
        return null;
    }

    @Override
    public String[] getSupportedCipherSuites(){ 
        return null;
    }

	@Override
	public Socket createSocket(String arg0, int arg1) throws IOException, UnknownHostException {
		// TODO Auto-generated method stub
		return null;
	}
	@Override
	public Socket createSocket(InetAddress arg0, int arg1) throws IOException {
		// TODO Auto-generated method stub
		return null;
	}
	@Override
	public Socket createSocket(String arg0, int arg1, InetAddress arg2, int arg3) throws IOException, UnknownHostException {
		// TODO Auto-generated method stub
		return null;
	}
	@Override
	public Socket createSocket(InetAddress arg0, int arg1, InetAddress arg2, int arg3) throws IOException {
		// TODO Auto-generated method stub
		return null;
	}
}