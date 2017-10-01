function requiresHTTPS(domain) {
  return true;
}

function redirect(requestDetails) {
  const domain = requestDetails.url.split('/')[2];
  if (requiresHTTPS(domain)) {
    console.log("Domain " + domain + " requires HTTPS");
  }
}

browser.webRequest.onBeforeRequest.addListener(
  redirect,
  {urls: ["http://*/*"]},
  ["blocking"]
);