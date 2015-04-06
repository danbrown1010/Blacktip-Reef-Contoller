package com.blacktipreef.blacktip;

import java.io.IOException;
import java.util.Date;

import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.google.appengine.api.datastore.DatastoreService;
import com.google.appengine.api.datastore.DatastoreServiceFactory;
import com.google.appengine.api.datastore.Entity;
import com.google.appengine.api.datastore.Key;
import com.google.appengine.api.datastore.KeyFactory;
import com.google.appengine.api.users.User;
import com.google.appengine.api.users.UserService;
import com.google.appengine.api.users.UserServiceFactory;

@SuppressWarnings("serial")
public class SubmitTankServlet extends HttpServlet {
	  public void doPost(HttpServletRequest req, HttpServletResponse resp)
		      throws IOException {
		    UserService userService = UserServiceFactory.getUserService();
		    User user = userService.getCurrentUser();

		    String fishroomName = req.getParameter("fishroomName");
		    Key fishroomKey = KeyFactory.createKey("Fishroom", fishroomName);
		    String content = req.getParameter("content");
		    Date date = new Date();
		    Entity tank = new Entity("Tank", fishroomKey);
		    if (user != null) {
		      tank.setProperty("author_id", user.getUserId());
		      tank.setProperty("author_email", user.getEmail());
		    }
		    tank.setProperty("date", date);
		    tank.setProperty("content", content);

		    DatastoreService datastore = DatastoreServiceFactory.getDatastoreService();
		    datastore.put(tank);

		    resp.sendRedirect("/blacktip.jsp?fishroomName=" + fishroomName);
		  }
}
