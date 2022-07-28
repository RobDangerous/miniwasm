function create_thread(func) {

}

onmessage = async function(e) {
  console.log('onmessage');
  
  const mod = e.data.mod;
  const memory = e.data.memory;
  const func = e.data.func;
  const canvas = e.data.canvas;

  const gl = canvas.getContext("webgl");

  const importObject = {
    env: { memory },
    imports: {
      imported_func: arg => {},//console.log('thread: ' + arg),
      create_thread
    }
  };

  const instance = await WebAssembly.instantiate(mod, importObject);

  // pasted in from https://www.tutorialspoint.com/webgl/webgl_drawing_a_triangle.htm
  const vertices = [
    -0.5,0.5,0.0,
    -0.5,-0.5,0.0,
    0.5,-0.5,0.0, 
  ];
 
  const indices = [0,1,2];
 
  const vertex_buffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, vertex_buffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
  gl.bindBuffer(gl.ARRAY_BUFFER, null);
  const Index_Buffer = gl.createBuffer();
  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, Index_Buffer);
  gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(indices), gl.STATIC_DRAW);
  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);

  const vertCode =
    'attribute vec3 coordinates;' +

    'void main(void) {' +
       ' gl_Position = vec4(coordinates, 1.0);' +
    '}';
  
  const vertShader = gl.createShader(gl.VERTEX_SHADER);
  gl.shaderSource(vertShader, vertCode);
  gl.compileShader(vertShader);

 const fragCode =
    'void main(void) {' +
       ' gl_FragColor = vec4(0.0, 0.0, 0.0, 0.1);' +
    '}';
  
  const fragShader = gl.createShader(gl.FRAGMENT_SHADER);
  gl.shaderSource(fragShader, fragCode); 
  gl.compileShader(fragShader);
  const shaderProgram = gl.createProgram();
  gl.attachShader(shaderProgram, vertShader);
  gl.attachShader(shaderProgram, fragShader);
  gl.linkProgram(shaderProgram);
  gl.useProgram(shaderProgram);
  gl.bindBuffer(gl.ARRAY_BUFFER, vertex_buffer);
  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, Index_Buffer);
  const coord = gl.getAttribLocation(shaderProgram, "coordinates");
  gl.vertexAttribPointer(coord, 3, gl.FLOAT, false, 0, 0); 
  gl.enableVertexAttribArray(coord);

  function render(time) {
     gl.clearColor(0.5, 0.5, 0.5, 0.9);
     gl.enable(gl.DEPTH_TEST);
     gl.clear(gl.COLOR_BUFFER_BIT);
     gl.viewport(0,0,canvas.width,canvas.height);
     gl.drawElements(gl.TRIANGLES, indices.length, gl.UNSIGNED_SHORT,0);

     instance.exports[func]();

    requestAnimationFrame(render);
  }
  requestAnimationFrame(render);
};
