package com.blacktipreef.api;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.Calendar;

import javax.servlet.http.*;

@SuppressWarnings("serial")
public class BlacktipAPIServlet extends HttpServlet {
	public void doGet(HttpServletRequest req, HttpServletResponse resp)
			throws IOException {
        String str = req.getParameter("user_input");
        PrintWriter writer = resp.getWriter();
        writer.write("<html>\n");
        writer.write("<b>");
        writer.write("doGet(): " + str + ", Time: " + Calendar.getInstance().getTime().toString());
        writer.write("</b>");
        writer.write("</html>");
	}
	
	public void doPost(HttpServletRequest req, HttpServletResponse resp)
			throws IOException {
        String str = req.getParameter("user_input");
        PrintWriter writer = resp.getWriter();
        writer.write("<html>\n");
        writer.write("<b>");
        writer.write("doPost(): " + str + ", Time: " + Calendar.getInstance().getTime().toString());
        writer.write("</b>");
        writer.write("</html>");
	}
}
