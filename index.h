
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Surgeon_Mwaipopo Education</title>

<script src="https://cdn.jsdelivr.net/npm/@supabase/supabase-js@2"></script>

<style>
*{box-sizing:border-box}
body{
 margin:0;
 font-family:Arial,sans-serif;
 background:#f1f5f9;
 color:#172554;
}
header{
 background:#082f49;
 color:white;
 padding:22px 15px;
 text-align:center;
}
header h1{margin:0;font-size:25px}
header p{margin:8px 0 0}
.container{
 max-width:650px;
 margin:25px auto;
 padding:15px;
}
.card{
 background:white;
 padding:22px;
 border-radius:12px;
 margin-bottom:18px;
 box-shadow:0 3px 12px #0001;
}
h2{color:#075985}
input{
 width:100%;
 padding:13px;
 margin:8px 0;
 border:1px solid #cbd5e1;
 border-radius:7px;
 font-size:15px;
}
button{
 width:100%;
 padding:13px;
 margin:7px 0;
 background:#0369a1;
 color:white;
 border:0;
 border-radius:7px;
 font-size:16px;
 font-weight:bold;
 cursor:pointer;
}
button.secondary{background:#475569}
.hidden{display:none}
.message{padding:10px;background:#e0f2fe;border-radius:6px}
table{width:100%;border-collapse:collapse;font-size:13px}
td,th{padding:9px;border:1px solid #ddd;text-align:left}
footer{text-align:center;padding:20px;color:#64748b}
</style>
</head>

<body>

<header>
<h1>Surgeon_Mwaipopo Education</h1>
<p>Welcome to Our Education Platform</p>
</header>

<div class="container">

<div id="authArea">

<div class="card">
<h2>Student Registration</h2>

<input id="fullName" placeholder="Full Name">
<input id="phone" type="tel" placeholder="Phone Number">
<input id="regEmail" type="email" placeholder="Email Address">
<input id="regPassword" type="password" placeholder="Create Password">

<button onclick="register()">Register Now</button>
</div>

<div class="card">
<h2>Login</h2>

<input id="loginEmail" type="email" placeholder="Email Address">
<input id="loginPassword" type="password" placeholder="Password">

<button onclick="login()">Login</button>
<p id="authMessage"></p>
</div>

</div>

<div id="studentArea" class="card hidden">
<h2>Student Dashboard</h2>
<p>Welcome, <strong id="studentName"></strong></p>
<p id="studentEmail"></p>
<p id="studentPhone"></p>
<p>Your account is active.</p>
<button class="secondary" onclick="logout()">Logout</button>
</div>

<div id="adminArea" class="card hidden">
<h2>Admin Dashboard</h2>
<p>Welcome, Administrator.</p>
<h3>Registered Students</h3>
<div id="studentList">Loading students...</div>
<button onclick="loadStudents()">Refresh Students</button>
<button class="secondary" onclick="logout()">Logout</button>
</div>

</div>

<footer>
<p>© 2026 Surgeon_Mwaipopo Education</p>
<p>All Rights Reserved</p>
</footer>

<script>
const SUPABASE_URL =
'https://gdvdwxvcyxzolagqmolp.supabase.co';

const SUPABASE_KEY =
'sb_publishable_quWzIYk5v8p_9lRWBOp_VA_Ld72ox76';

const db = window.supabase.createClient(
SUPABASE_URL,
SUPABASE_KEY
);

function message(text){
 document.getElementById('authMessage').textContent=text;
}

async function register(){
 const fullName=document.getElementById('fullName').value.trim();
 const phone=document.getElementById('phone').value.trim();
 const email=document.getElementById('regEmail').value.trim();
 const password=document.getElementById('regPassword').value;

 if(!fullName||!phone||!email||!password){
  alert('Please fill in all fields.');
  return;
 }

 if(password.length<6){
  alert('Password must contain at least 6 characters.');
  return;
 }

 const {data,error}=await db.auth.signUp({
  email:email,
  password:password,
  options:{
   data:{
    full_name:fullName,
    phone:phone
   }
  }
 });

 if(error){
  alert(error.message);
  return;
 }

 alert('Registration submitted successfully. Check your email if confirmation is required.');
}

async function login(){
 const email=document.getElementById('loginEmail').value.trim();
 const password=document.getElementById('loginPassword').value;

 if(!email||!password){
  message('Enter email and password.');
  return;
 }

 const {data,error}=await db.auth.signInWithPassword({
  email:email,
  password:password
 });

 if(error){
  message(error.message);
  return;
 }

 await showDashboard(data.user);
}

async function showDashboard(user){
 document.getElementById('authArea').classList.add('hidden');

 const {data:profile,error}=await db
  .from('profiles')
  .select('id,email,full_name,phone,role')
  .eq('id',user.id)
  .maybeSingle();

 if(error||!profile){
  alert('Profile not found. Please contact the administrator.');
  await db.auth.signOut();
  location.reload();
  return;
 }

 if(profile.role==='admin'){
  document.getElementById('adminArea').classList.remove('hidden');
  await loadStudents();
 }else{
  document.getElementById('studentArea').classList.remove('hidden');
  document.getElementById('studentName').textContent=
   profile.full_name||'Student';
  document.getElementById('studentEmail').textContent=
   'Email: '+(profile.email||user.email);
  document.getElementById('studentPhone').textContent=
   'Phone: '+(profile.phone||'Not provided');
 }
}

async function loadStudents(){
 const area=document.getElementById('studentList');
 area.textContent='Loading students...';

 const {data,error}=await db
  .from('profiles')
  .select('full_name,email,phone,role')
  .eq('role','student')
  .order('full_name');

 if(error){
  area.textContent='Unable to load students: '+error.message;
  return;
 }

 if(!data||data.length===0){
  area.textContent='No students registered yet.';
  return;
 }

 let html='<table><tr><th>Name</th><th>Email</th><th>Phone</th></tr>';

 data.forEach(student=>{
  html+='<tr>'+
  '<td>'+escapeHTML(student.full_name||'')+'</td>'+
  '<td>'+escapeHTML(student.email||'')+'</td>'+
  '<td>'+escapeHTML(student.phone||'')+'</td>'+
  '</tr>';
 });

 html+='</table>';
 area.innerHTML=html;
}

function escapeHTML(value){
 return String(value).replace(/[&<>"']/g,function(c){
  return {'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c];
 });
}

async function logout(){
 await db.auth.signOut();
 location.reload();
}

async function checkSession(){
 const {data}=await db.auth.getSession();

 if(data.session){
  await showDashboard(data.session.user);
 }
}

checkSession();

</script>
</body>
</html>
