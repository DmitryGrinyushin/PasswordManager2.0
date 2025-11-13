using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Newtonsoft.Json;

namespace PasswordManagerWeb.Controllers
{
    public class UserController : Controller
    {
        private readonly HttpClient _httpClient;

        public UserController()
        {
            _httpClient = new HttpClient();
        }

        [HttpGet]
        public IActionResult Login()
        {
            return View();
        }

        [HttpPost]
        public async Task<IActionResult> Login(string username, string password)
        {
            // Prepare JSON payload
            var loginData = new { username = username, password = password };
            var json = JsonConvert.SerializeObject(loginData);
            var content = new StringContent(json, Encoding.UTF8, "application/json");

            try
            {
                // Send POST request to backend API
                var response = await _httpClient.PostAsync("http://localhost:8080/login", content);

                if (response.IsSuccessStatusCode)
                {
                    var responseContent = await response.Content.ReadAsStringAsync();
                    dynamic? result = JsonConvert.DeserializeObject(responseContent);

                    if (result == null)
                    {
                        ViewData["Error"] = "Failed to parse response from server.";
                        return View();
                    }
            
                    string token = result.token;

                    ViewData["Message"] = $"Login successful! Token: {token}";

                    // TODO: Save token in cookie or session

                    return View();
                }
                else
                {
                    ViewData["Error"] = $"Login failed: {response.StatusCode}";
                    return View();
                }
            }
            catch (HttpRequestException e)
            {
                ViewData["Error"] = $"Request error: {e.Message}";
                return View();
            }
        }
    }
}